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
void H3Grid::processSample(double height)
{
    // bins for H3 auto resolution:
    // - H3 level ~roughly~ equivalent to hexer hexagon size at same edge value
    //     - (since our coords are in degrees, the appropriate values will vary based on
    //       location. Some way of scaling this by latitude would be more accurate)
    // - does not automatically make very large (>1km^2) or very small (<6m^2) hexagons
    // We ignore resolutions 1 through 7, so add 8 to the entry we find..
    const std::array<double, 7> resHeights { 2.0, 2.62e-4, 6.28e-5, 2.09e-5, 8.73e-6,
                                             3.32e-6, 1.4e-6 };

    for (int i = 0; i < resHeights.size(); ++i) {
        if (height < resHeights[i])
            m_res = i + 8;
    }
    if (m_res == -1)
        throw hexer_error("unable to calculate H3 grid size!");
    
    std::cout << "H3 resolution: " << m_res << std::endl; 
}

HexId H3Grid::findHexagon(Point p)
{
    H3Index index(0);
    LatLng ll{p.m_y, p.m_x};
    if (latLngToCell(&ll, m_res, &index) != E_SUCCESS)
        throw hexer_error("H3 index not found!");
    if (m_origin == 0)
        m_origin = index;
    return h32ij(index);
}

HexId H3Grid::edgeHex(HexId hex, int edge)
{
    // these offsets are in the same order as hexer and not H3Grid. need to be careful in findShape
    const HexId offsets[] {{1, 0}, {1, 1}, {0, 1}, {-1, 0}, {-1, -1}, {0, -1}};
    return hex + offsets[edge]; 
}

} // namespace hexer