/* #pragma once

#include <string>

#include <h3/include/h3api.h>

#include "Hexagon.hpp"
#include "exception.hpp"
#include "export.hpp"

namespace hexer {

class H3Grid;

class HEXER_DLL H3Hex : public Hexagon
{

public:
    H3Hex(CoordIJ ij)
        : m_ij{ij}
    {}
    

private:
    ///
    CoordIJ m_ij;
};
} // namespace hexer  */