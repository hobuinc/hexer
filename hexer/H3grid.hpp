#pragma once

#include <map>
#include <vector>
#include <sstream>

#include <hexer/hexer.hpp>

#include "Mathpair.hpp"
#include "export.hpp"
#include "H3Path.hpp"
#include "BaseGrid.hpp"
#include "HexId.hpp"

#include <h3/include/h3api.h>

/* static bool operator<(HexId const& c1, HexId const& c2)
    {
        return (c1.i < c2.i) || ((c1.i == c2.i) && (c1.j < c2.j));
    }
static bool operator==(HexId const& c1, HexId const& c2)
    {   
        return ((c1.i == c2.i) && (c1.j == c2.j)); 
    }
 */
namespace hexer 
{

using DirEdge = H3Index;

static HexId operator+(HexId const& c1, HexId const& c2)
    {   return {c1.i + c2.i, c1.j + c2.j};  }

class HEXER_DLL H3Grid : protected BaseGrid
{
public:
    H3Grid(int res, int dense_limit)
        : BaseGrid{dense_limit}, m_res{res}
    {}
    
    H3Index ij2h3(HexId ij)
        {   H3Index h3;
            if (localIjToCell(m_origin, &ij, 0, &h3) != E_SUCCESS) {
                std::ostringstream oss;
                oss << "Can't convert IJ (" << std::to_string(ij.i) << 
                    ", " << std::to_string(ij.j) <<") to H3Index.";
                throw hexer_error(oss.str());
            }
            return h3;  }

    // Convert H3 index to IJ coordinates
    HexId h32ij(H3Index h3)
        {   HexId ij;
            if (cellToLocalIj(m_origin, h3, 0, &ij) != E_SUCCESS) {
                std::ostringstream oss;
                oss << "Can't convert H3 index " << std::to_string(h3) << 
                    " to IJ.";
                throw hexer_error(oss.str()); 
            }
            return ij;  }    
    
    HexId findHexagon(Point p);
    HexId edgeHex(HexId hex, int edge);

private:
    void processSample(double height);

    std::vector<Point> m_samples;
    int m_res;
    H3Index m_origin{0};

};

} // namepsace hexer


