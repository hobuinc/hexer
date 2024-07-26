#include <assert.h>
#include <map>
#include <iostream>

#include <hexer/H3grid.hpp>

#include <h3/include/h3api.h>

namespace hexer 
{

    // add stuff for processing unknown resolution
    
    void H3Grid::addLatLng(LatLng *ll)
    {
        H3Index index(0);
        H3Error err = latLngToCell(ll, m_res, &index);

        if (err == E_SUCCESS) {
            ++m_map[index];
        }
        else {
            std::cerr << "H3 cell index not found!" << std::endl;
        }
    }
} // namespace hexer