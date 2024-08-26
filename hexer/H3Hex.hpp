#pragma once

#include <string>

#include <h3/include/h3api.h>

#include "Hexagon.hpp"
#include "exception.hpp"
#include "export.hpp"

namespace hexer {

class HEXER_DLL H3Hex : public Hexagon
{

public:
    H3Hex()
    H3Index ij2h3(CoordIJ ij)
        {   H3Index h3;
            if (localIjToCell(m_origin, &ij, 0, &h3) != E_SUCCESS) {
                std::ostringstream oss;
                oss << "Can't convert IJ (" << std::to_string(ij.i) << 
                    ", " << std::to_string(ij.j) <<") to H3Index.";
                throw hexer_error(oss.str());
            }
            return h3;  }

    // Convert H3 index to IJ coordinates
    CoordIJ h32ij(H3Index h3)
        {   CoordIJ ij;
            if (cellToLocalIj(m_origin, h3, 0, &ij) != E_SUCCESS) {
                std::ostringstream oss;
                oss << "Can't convert H3 index " << std::to_string(h3) << 
                    " to IJ.";
                throw hexer_error(oss.str()); 
            }
            return ij;  }

    // Return the IJ coordinate of the next cell we're checking for density (going clockwise).
    CoordIJ nextCoord(CoordIJ ij, int edge)
        {   edge++;
            if (edge == 6)
                edge = 0;
            return edgeCoord(ij, edge);  }

    // Return the IJ coordinate of the cell across the edge.
    CoordIJ edgeCoord(CoordIJ ij, int edge)
        {   const CoordIJ offsets[] {{1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {0, 1}, {1, 1}};
            return ij + offsets[edge];  }

private:
    ///
    CoordIJ m_ij;
};
} // namespace hexer