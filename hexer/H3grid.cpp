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
        if (latLngToCell(ll, m_res, &index) != E_SUCCESS)
           throw hexer_error("H3 index not found!"); 
        if (m_map.size() == 0)
            m_origin = index;
        ++m_map[index];
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
        int counter(0);
        while (!m_possible.empty()) {
            std::vector<DirEdge> shape = findShape();
            m_boundary.push_back(shape);
            std::cout << "s size: " << shape.size() << "; iteration " << counter << "\n" << std::endl;
            counter++;
        }
    }

    void H3Grid::possible(H3Index idx)
    {
        CoordIJ c;
        if (cellToLocalIj(m_origin, idx, 0, &c) != E_SUCCESS) 
            throw hexer_error("Cannot find IJ coordinates!");
        
        CoordIJ shift = edgeCoord(c, 0);
        H3Index shift_idx;
        if (localIjToCell(m_origin, &shift, 0, &shift_idx) != E_SUCCESS) 
            throw hexer_error("Cannot find neighbor H3 cell!");

        if (m_map.find(shift_idx) == m_map.end())
            m_possible[idx] = c;
        else
            return;
    }

    std::vector<DirEdge> H3Grid::findShape()
    {
        std::vector<DirEdge> s;
        int edge(0);
        CoordIJ cur = m_possible.begin()->second;
        const CoordIJ orig = m_possible.begin()->second;
        int in_counter(0);
        do {
            std::cout << "edge at START of loop: " << edge << std::endl;
            if (edge == 0)
                m_possible.erase(ij2h3(cur));
            addEdge(s, cur, edge);
            CoordIJ next = nextCoord(cur, edge);
            std::cout << "next ; " << next.i << ", " << next.j << std::endl;
            // if next is dense: go right
            if (m_map.find(ij2h3(next)) != m_map.end()) {
                cur = next;
                edge--;
                if (edge < 0)
                    edge = 5;
            }
            else {
                if (edge == 5)
                    edge = 0;
                else
                    edge++;
            }
            std::cout << in_counter << " . " << s[in_counter] << std::endl;
            in_counter++;
            std::cout << "edge at END of loop: " << edge << "\n"<< std::endl;
        } while (!(cur == orig && edge == 0));
        return s;
    }

/*    int H3Grid::walkBounds(int edge, std::vector<H3Index>& s) 
    {
        addEdge(s, m_cur, edge);
        if (edge == 0)
            m_possible.erase(ij2h3(m_cur));
        CoordIJ next = nextCoord(m_cur, edge);
        // if next is dense: go left
        if (m_map.find(ij2h3(next)) != m_map.end()) {
            m_cur = next;
            edge--;
            if (edge < 0)
                edge = 5;
        }
        else
            edge++;

        return edge;
    }  */

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
        std::vector<CoordIJ> offsets {{1, 1}, {0, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, 0}}; //{{1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {0, 1}, {1, 1}};
        return ij + offsets[edge];  
    }

    void H3Grid::addEdge(std::vector<DirEdge>& s, CoordIJ idx, int edge)
    {
        H3Index src = ij2h3(idx);
        CoordIJ next_ij = edgeCoord(idx, edge);
        DirEdge dirEdge;
        std::cout << "current: " << idx.i << ", " << idx.j << "; across: " << next_ij.i << ", " << next_ij.j << std::endl;
        if (cellsToDirectedEdge(src, ij2h3(next_ij), &dirEdge) != E_SUCCESS)
            throw hexer_error("Couldn't get directed edge.");
        s.push_back(dirEdge); 
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
            if (cellToLocalIj(m_origin, it->first, 0, &c) != E_SUCCESS) 
                throw hexer_error("H3 index not found!");
            std::ostringstream coords;
            coords << "(" << (int)c.i <<
                ", " << (int)c.j << ")";
            m_ij_coords.push_back(coords.str());
        } 
    }

} // namespace hexer