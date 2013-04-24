/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Howard Butler, hobu.inc@gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#ifndef INCLUDED_PSHAPE_OGR_HPP
#define INCLUDED_PSHAPE_OGR_HPP

#include <hexer/hexer.hpp>
#include <hexer/hexer_defines.h>

#ifdef HEXER_HAVE_GDAL

#include "ogr_api.h"
#include "gdal.h"

#endif

#include "Mathpair.hpp"
#include "export.hpp"

namespace hexer
{

class HexGrid;
class Hexagon;
class Segment;


class HEXER_DLL OGR
{
public:
    OGR(HexGrid *grid_p, std::string const& filename);
    ~OGR();

    void drawHexagon(Hexagon *hex_p, bool fill = false);
    void drawSegment(Segment s);
    void drawPoint(Point p);

private:
    HexGrid *m_grid_p;
    std::string m_filename;


};

} // namespace

#endif // file guard
