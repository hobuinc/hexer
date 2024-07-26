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
        : m_dense_limit{dense_limit}, m_res{resolution}, m_map{H3Map()}
    {}
    void addLatLng(LatLng *ll);
    std::map<H3Index, int> getmap() const 
        { return m_map; }
    //void processPoint();
private:
    int m_count;
    typedef std::map<H3Index, int> H3Map;
    int m_res;
    int m_dense_limit;
    H3Map m_map;
    H3Index m_idx;
    LatLng m_loc;
};

} // namepsace hexer