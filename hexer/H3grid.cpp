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
            if (m_map.size() == 0)
                m_origin = index;
            ++m_map[index];
        }
        else {
            throw hexer_error("H3 index not found!");
        }
    }

    void H3Grid::processGrid()
    {
        for (auto it = m_map.begin(); it != m_map.end();) {
            if (it->second < m_dense_limit)
                it = m_map.erase(it);
            else {
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
        if (cellToLocalIj(m_origin, idx, 0, &c) != E_SUCCESS) 
            throw hexer_error("Cannot find IJ coordinates!");
        
        CoordIJ shift;
        shift.i = c.i + 1;
        shift.j = c.j;
        H3Index shift_idx;
        if (localIjToCell(m_origin, &shift, 0, &shift_idx) != E_SUCCESS) 
            throw hexer_error("Cannot find neighbor H3 cell!");

        if (m_map.find(shift_idx) == m_map.end())
            m_possible[idx] = std::pair(c, shift);
        else
            return;
    }

    void H3Grid::walkBounds()
    {
        for(auto it = m_possible.begin(); it != m_possible.end();) {
            Shape s;
            CoordIJ cur = it->second.first;
            CoordIJ next = it->second.second;
            int edge(0);
            H3Index start_idx;
            H3Index cur_idx = it->first;
            H3Index next_idx = ij2h3(next);
            H3Index cur_edge;

            if (localIjToCell(m_origin, &it->second.first, 0, &start_idx) != E_SUCCESS) 
                throw hexer_error("Cannot find neighbor H3 cell!"); 
            if (cellsToDirectedEdge(cur_idx, next_idx, &cur_edge) != E_SUCCESS)
                throw hexer_error("Cannot find directed edge!");
            s.push_back(cur_edge);
            edge++;

            while (cur != it->second.first && edge != 0) {
                if (edge == 0) {
                    next = edgeCoord(cur, 0);
                    next_idx = ij2h3(next);
                    if (m_map.find(next_idx) == m_map.end()) {
                        addEdge(s, cur, edge);
                        m_possible.erase(ij2h3(cur));
                        edge++;
                    }
                    else {
                        cur = next;
                        edge = 4;
                    }
                }
                if (edge == 1) {
                    next = edgeCoord(cur, 1);
                    next_idx = ij2h3(next);
                    if (m_map.find(next_idx) == m_map.end()) {
                        addEdge(s, cur, edge);
                        edge++;
                    }
                    else {
                        cur = next;
                        edge = 5;
                    }
                }
            }
            m_boundary.push_back(s);
            m_possible.erase(it);
        }
    }
    // Return the IJ coordinate of the next cell we're checking for density (going clockwise).
    CoordIJ H3Grid::nextCoord(CoordIJ ij, int edge)
    {
        edge++;
        if (edge == 6)
            edge = 0;
        return edgeCoord(ij, edge);  
    }

    // Return the IJ coordinate of the cell across the edge.
    CoordIJ H3Grid::edgeCoord(CoordIJ ij, int edge)
    {
        std::vector<CoordIJ> offsets {{1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {0, 1}, {1, 1}};
        return ij + offsets[edge];  
    }

    void H3Grid::addEdge(Shape s, CoordIJ idx, H3Index next)
    {
        H3Index src = ij2h3(idx);
        H3Index dirEdge;
        if (cellsToDirectedEdge(src, next, &dirEdge) != E_SUCCESS)
            throw hexer_error("Couldn't get directed edge.");
        s.push_back(dirEdge); 
    }
/*
    Shape H3Grid::findShape()
    {
        Shape s;

//
//     __3_
//  2 /    \ 4
//   /      \
//   \      /
//  1 \____/ 5
//      0
//

        if (m_possible.empty())
            return s;

        CoordIJ start = m_possible.front().second.first;
        CoordIJ edge = start;
        CoordIJ idx = 

        int edge = 0;
        addEdge(s, idx, edge);
        while (idx != start && edge != 0)
        {
            s.addEdge(idx, edge);
            // If we traversed a possible root, remove it as we've dealt with its potential
            // shape.
            if (edge == 0)
                m_possible.erase(ij2h3(idx));
            CoordIJ next = nextCoord(idx, edge);
            if (isDense(next)) // Go left
            {
                idx = next;
                edge--;
                if (edge < 0)
                    edge = 5;
            }
            else  // Go right
                edge++;
        }
        return s;
    } */

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
            if (cellToLocalIj(m_origin, it->first, 0, &c) != E_SUCCESS) 
                throw hexer_error("H3 index not found!");
            std::ostringstream coords;
            coords << "(" << (int)c.i <<
                ", " << (int)c.j << ")";
            m_ij_coords.push_back(coords.str());
        } 
    }

} // namespace hexer