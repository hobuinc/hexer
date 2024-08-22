#pragma once

#include <map>
#include <vector>
#include <sstream>

#include <hexer/hexer.hpp>

#include "Mathpair.hpp"
#include "export.hpp"
#include "H3Path.hpp"

#include <h3/include/h3api.h>

static bool operator<(CoordIJ const& c1, CoordIJ const& c2)
    {
        return (c1.i < c2.i) || ((c1.i == c2.i) && (c1.j < c2.j));
    }
static bool operator==(CoordIJ const& c1, CoordIJ const& c2)
    {   
        return ((c1.i == c2.i) && (c1.j == c2.j)); 
    }

namespace hexer 
{

using DirEdge = H3Index;

static CoordIJ operator+(CoordIJ const& c1, CoordIJ const& c2)
    {   return {c1.i + c2.i, c1.j + c2.j};  }

class HEXER_DLL H3Grid
{
public:
    H3Grid(int dense_limit, int resolution)
        : m_dense_limit{dense_limit}, m_res{resolution},
          m_maxSample{10000}, m_origin{0}
    {}

    ~H3Grid()
    {
        for (std::vector<H3Path*>::size_type i = 0; i < m_paths.size(); i++)
            delete m_paths[i];
    }

    void addLatLng(LatLng *ll);
    std::map<CoordIJ, int> getMap() const 
        { return m_map; }
    int getRes() const
        { return m_res; }
    void setSampleSize(unsigned sampleSize)
        { m_maxSample = sampleSize; }
    std::vector<H3Path *> const& rootPaths() const
        { return m_paths; }
    int numHex()
        { return m_numdense; }
    // debug function, add grid for testing
    void setGrid(std::pair<CoordIJ, int> cell)
        { m_map.insert(cell); }
    // debug function, specify origin for H3 grid
    void setOrigin(H3Index idx)
        { m_origin = idx; }
    void processGrid();
    void processPaths();

    // Convert IJ coordinates to H3 index
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
    void organizePaths();
    void parentOrChild(H3Path *p);
    void processH3Sample();
    void findShape();
    void addEdge(H3Path * p, CoordIJ idx, int edge);

    /// minimum I in IJ coordinates of grid
    int m_min_i;
    /// Count of points in H3-indexed hexagons
    int m_count;
    /// Map of hexagons based on IJ coordinates
    typedef std::map<CoordIJ, int> H3Map;
    H3Map m_map;
    /// IJ coordinates of hexagons with non-dense neighbors at side 0 
    std::vector<CoordIJ> m_possible;
    /// H3 resolution of grid (0-15); -1 for auto edge length calculation
    int m_res;
    /// Number of points that must lie in a hexagon for it to be interesting 
    int m_dense_limit;
    /// Vector of points to use to determine hex height.
    std::vector<LatLng> m_sample;
    /// Maximum sample size.
    unsigned m_maxSample;
    /// Origin H3 index for finding IJ coordinates
    H3Index m_origin;
    /// List of paths
    std::vector<H3Path *> m_paths;
    /// map of cell coordinates with associated paths at edge 0
    typedef std::map<CoordIJ, H3Path *> IJPathMap;
    IJPathMap m_hex_paths;
    /// number of dense h3 cells in the grid
    int m_numdense;

};

} // namepsace hexer

