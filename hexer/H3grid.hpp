#pragma once

#include <map>
#include <vector>
#include <sstream>


#include "Mathpair.hpp"
#include "export.hpp"
#include "BaseGrid.hpp"
#include "HexId.hpp"
#include "exception.hpp"

#include <h3/include/h3api.h>

namespace hexer
{

using DirEdge = H3Index;

class H3Grid : public BaseGrid
{
public:
    H3Grid(int dense_limit)
        : BaseGrid{dense_limit}, m_res{-1}, m_origin{0}
        {}
    H3Grid(int res, int dense_limit)
        : BaseGrid{dense_limit}, m_res{res}, m_origin{0}
        {}

    H3Index ij2h3(HexId ij)
        {   H3Index h3;
            if (localIjToCell(m_origin, &ij, 0, &h3) != E_SUCCESS) {
                std::ostringstream oss;
                oss << "Can't convert IJ (" << ij.i <<
                    ", " << ij.j <<") to H3Index.";
                throw hexer_error(oss.str());
            }
            return h3;  }

    // Convert H3 index to IJ coordinates
    HexId h32ij(H3Index h3)
        {   HexId ij;
            if (cellToLocalIj(m_origin, h3, 0, &ij) != E_SUCCESS) {
                std::ostringstream oss;
                oss << "Can't convert H3 index " << h3 <<
                    " to IJ.";
                throw hexer_error(oss.str());
            }
            return ij;  }

    HexId findHexagon(Point p);
    HexId edgeHex(HexId hex, int edge) const;
    Point findPoint(Segment& s);

    void addXY(double& x, double& y)
        { 
          Point p{degsToRads(x), degsToRads(y)};
          addPoint(p);        
        }
    bool sampling() const
        { return m_res < 0; }
    bool inGrid(HexId& h)
        { return h.i >= m_minI; }
    HexId moveCoord(HexId& h)
        { return HexId{h.i - 1, h.j}; }
    bool isH3()
        { return true; }

    // test function: used when inserting pre-defined grids in tests, 
    // sets origin outside of findHexagon()
    void setOrigin(H3Index idx)
        { m_origin = idx; }
    // test function: used to get grid resolution to run h3 latLngToCell()
    int getRes() const
        { return m_res; }

private:
    void processHeight(double height);

    /// H3 resolution of the grid (0-15)
    int m_res;
    /// minimum I value for iterating through parent paths
    int m_minI;
    /// origin index for converting between H3Index and CoordIJ
    H3Index m_origin;

};

} // namepsace hexer


