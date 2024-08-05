#include <assert.h>
#include <map>
#include <iostream>
#include <sstream>

#include <hexer/H3grid.hpp>
#include <hexer/exception.hpp>
#include <hexer/Processor.hpp>

#include <h3/include/h3api.h>

namespace hexer 
{

    // add stuff for processing unknown resolution
    
    void H3Grid::addLatLng(LatLng *ll)
    {
        if(m_res == -1) {
            m_sample.push_back(*ll);
            if (m_sample.size() >= m_maxSample)
                processH3Sample();
            return;
        }

        H3Index index(0);
        H3Error err = latLngToCell(ll, m_res, &index);

        if (err == E_SUCCESS) {
            if (m_map.size() == 0) {
                m_origin = index;
            }
            ++m_map[index];
        }
        else {
            throw hexer_error("H3 index not found!");
        }
    }

    void H3Grid::processGrid()
    {
        for(auto it = m_map.begin(); it != m_map.end();) {
            if(it->second < m_dense_limit)
                it = m_map.erase(it);
            else{
                possible(it->first);
                ++it;
            }
        }
        if (m_map.empty()) {
            throw hexer_error("No areas of sufficient density - no shapes. "
                "Decrease density or area size.");
        }
        std::cerr << m_map.size() << " . " << m_possible.size() << std::endl;
    }

    void H3Grid::possible(H3Index idx)
    {
        CoordIJ c;
        H3Error er = cellToLocalIj(m_origin, idx, 0, &c);
        if (er == E_SUCCESS) {
            CoordIJ shift;
            shift.i = c.i + 1;
            shift.j = c.j;
            H3Index shift_idx;
            H3Error err = localIjToCell(m_origin, &shift, 0, &shift_idx);
            if (err == E_SUCCESS) {
                if (m_map.find(shift_idx) == m_map.end())
                    m_possible[idx] = c;
                else
                    return;
            }
            else
                throw hexer_error("Cannot find neighbor H3 cell!");
        }
        else
            throw hexer_error("Cannot find IJ coordinates!");
    }

    void H3Grid::walkBounds()
    {
        for(auto it = m_possible.begin(); it != m_possible.end();) {

        }
    }

    void H3Grid::processH3Sample()
    {
        if (m_res > 0 || m_sample.empty())
            return;
        double height = computeHexSize(m_sample, m_dense_limit);

        // bins for H3 auto resolution:
        // - H3 level ~roughly~ equivalent to hexer hexagon size at same edge value
        //     - (since our coords are in degrees, the appropriate values will vary based on
        //       location. Some way of scaling this by latitude would be more accurate)
        // - does not automatically make very large (>1km^2) or very small (<6m^2) hexagons
        // We ignore resolutions 1 through 7, so add 8 to the entry we find..
        const std::array<double, 6> resHeights { 2.62e-4, 6.28e-5, 2.09e-5, 8.73e-6, 3.32e-6, 1.4e-6 };

        for (int i = 0; i < resHeights.size(); ++i) {
            if (height > resHeights[i])
                m_res = i + 8;
        }
        if (m_res == -1)
            throw hexer_error("unable to calculate H3 grid size!");
        
        for (auto pi = m_sample.begin(); pi != m_sample.end(); ++pi) {
            LatLng ll = *pi;
            addLatLng(&ll);
        }
        m_sample.clear();
        std::cout << "res: " << m_res <<std::endl; 
    }

    void H3Grid::findIJ()
    {
        for(auto it = m_map.begin(); it != m_map.end(); ++it) {
            CoordIJ c;
            H3Error err = cellToLocalIj(m_origin, it->first, 0, &c);
            if (err == E_SUCCESS) {
                std::ostringstream coords;
                coords << "(" << (int)c.i <<
                    ", " << (int)c.j << ")";
                m_ij_coords.push_back(coords.str());
            }
            else {
                throw hexer_error("H3 index not found!");
            } 
        } 
    }

} // namespace hexer