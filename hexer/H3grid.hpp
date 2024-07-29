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
          m_maxSample{10000}
    {}
    void addLatLng(LatLng *ll);
    std::map<H3Index, int> getMap() const 
        { return m_map; }
    int getRes() const
        { return m_res; }
    void setSampleSize(unsigned sampleSize)
        { m_maxSample = sampleSize; }
    void processGrid();
private:
    void processH3Sample();
    /// Count of points in H3-indexed hexagons
    int m_count;
    /// Map of hexagons based on H3 index
    typedef std::map<H3Index, int> H3Map;
    H3Map m_map;
    /// H3 resolution of grid (0-15); -1 for auto edge length calculation
    int m_res;
    /// Number of points that must lie in a hexagon for it to be interesting 
    int m_dense_limit;
    /// Vector of points to use to determine hex height.
    std::vector<Point> m_sample;
    /// Maximum sample size.
    unsigned m_maxSample;

    H3Index m_idx;
    LatLng m_loc;
};

} // namepsace hexer