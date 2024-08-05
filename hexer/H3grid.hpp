#pragma once

#include <map>
#include <vector>

#include "Mathpair.hpp"
#include "export.hpp"

#include <h3/include/h3api.h>

namespace hexer 
{

class HEXER_DLL H3Grid
{
public:
    H3Grid(int dense_limit, int resolution)
        : m_dense_limit{dense_limit}, m_res{resolution}, m_map{H3Map()},
          m_maxSample{10000}, m_origin{0}
    {}
    void addLatLng(LatLng *ll);
    std::map<H3Index, int> getMap() const 
        { return m_map; }
    int getRes() const
        { return m_res; }
    std::vector<std::string> getIJArr() const
        { return m_ij_coords; }
    void setSampleSize(unsigned sampleSize)
        { m_maxSample = sampleSize; }
    void processGrid();
    void findIJ();
private:
    void processH3Sample();
    void possible(H3Index idx);
    void walkBounds();
    bool coordEq(CoordIJ a, CoordIJ b);

    /// Count of points in H3-indexed hexagons
    int m_count;
    /// Map of hexagons based on H3 index
    typedef std::map<H3Index, int> H3Map;
    H3Map m_map;
    /// hexagons with non-dense neighbors
    std::map<H3Index, CoordIJ> m_possible;
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
    /// Vector of localIJ coordinates as strings, in the same order as m_map
    std::vector<std::string> m_ij_coords;

    H3Index m_idx;
    LatLng m_loc;
};

} // namepsace hexer