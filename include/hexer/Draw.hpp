/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Andrew Bell andrew.bell.ia at gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#ifndef INCLUDED_PSHAPE_DRAW_HPP
#define INCLUDED_PSHAPE_DRAW_HPP

#include <hexer/hexer.hpp>


#ifdef HEXER_HAVE_CAIRO
#include <cairo/cairo.h>
#include <cairo/cairo-svg.h> 
#endif

#include "Mathpair.hpp"

namespace Pshape
{

class HexGrid;
class Hexagon;
class Segment;

struct Color
{
    Color(double red, double blue, double green) :
        m_red(red), m_blue(blue), m_green(green)
    {}

    double m_red;
    double m_blue;
    double m_green;
};

class Draw
{
public:
    Draw(HexGrid *grid_p);
    ~Draw();

    void drawHexagon(Hexagon *hex_p, bool fill = false);
    void drawSegment(Segment s, Color c = Color(0, 0, 1));
    void drawPoint(Point p);

private:
    HexGrid *m_grid_p;
    // Display *m_dpy_p;
    // Window m_window;
#ifdef HEXER_HAVE_CAIRO
    
    cairo_surface_t *m_surface_p;
    cairo_t *m_cairo_p;
#endif
    
};

} // namespace

#endif // file guard
