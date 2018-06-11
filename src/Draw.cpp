/******************************************************************************
 * Copyright (c) 2014, Hobu Inc. (howard@hobu.co)
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following
 * conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of the Howard Butler or Hobu, Inc.
 *       the names of its contributors may be
 *       used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 ****************************************************************************/

#include <hexer/Draw.hpp>

#include <hexer/Hexagon.hpp>
#include <hexer/HexGrid.hpp>
#include <hexer/Mathpair.hpp>
#include <hexer/Segment.hpp>
#include <hexer/hexer_defines.h>

using namespace std;

double red[] = { 1, 0, 0 };
double yellow[] = { 1, 1, 0 };
double blue[] = { 0, 0, 1 };

typedef double Color[];

namespace hexer
{

Draw::Draw( HexGrid *grid_p, std::string const& filename) :
    m_grid_p(grid_p), m_filename(filename)
{

#ifdef HEXER_HAVE_CAIRO
    m_surface_p = cairo_svg_surface_create(m_filename.c_str(), 750, 1000);
    m_cairo_p = cairo_create(m_surface_p);

    // Move things to the center.
    cairo_translate(m_cairo_p, 400, 50 );
#endif
}

Draw::~Draw()
{
#ifdef HEXER_HAVE_CAIRO
    cairo_surface_destroy(m_surface_p);
    cairo_destroy(m_cairo_p);
#endif

}

void Draw::drawHexagon(Hexagon *hex_p, bool fill)
{
#ifdef HEXER_HAVE_CAIRO
    Point pos;

    cairo_set_line_width(m_cairo_p, 1);
    cairo_set_source_rgb(m_cairo_p, .5, .5, 0);
    pos.m_x = hex_p->x() * m_grid_p->width();
    pos.m_y = hex_p->y() * m_grid_p->height();

    if (hex_p->xodd())
        pos.m_y += (m_grid_p->height() / 2);
    cairo_move_to(m_cairo_p, pos.m_x, pos.m_y);
    for (int i = 1; i <= 5; ++i)
    {
        Point p = pos + m_grid_p->offset(i);
        cairo_line_to(m_cairo_p, p.m_x, p.m_y);
    }
    cairo_close_path(m_cairo_p);
    if ((hex_p->x() == 0) && (hex_p->y() == 0))
        cairo_fill(m_cairo_p);

    if (hex_p->dense())
    {
        cairo_set_source_rgb(m_cairo_p, .3, .3, 0);
        cairo_fill(m_cairo_p);
    }
    if ( hex_p->possibleRoot())
    {
        Segment s(hex_p, 0);
        drawSegment(s, Color(1, 0, 0));
    }

    cairo_stroke(m_cairo_p);
    cairo_surface_flush(m_surface_p);
#endif
}

void Draw::drawSegment(Segment s, Color c)
{
#ifdef HEXER_HAVE_CAIRO
    cairo_set_line_width(m_cairo_p, 2);
    cairo_set_source_rgb(m_cairo_p, c.m_red, c.m_blue, c.m_green);

    hexer::Point startpoint = s.startPos(m_grid_p) - m_grid_p->origin();
    hexer::Point endpoint = s.endPos(m_grid_p) - m_grid_p->origin();
    cairo_move_to(m_cairo_p, startpoint.m_x, startpoint.m_y);
    cairo_line_to(m_cairo_p, endpoint.m_x, endpoint.m_y);
    cairo_stroke(m_cairo_p);
    cairo_surface_flush(m_surface_p);
#endif
}

void Draw::drawPoint(hexer::Point p)
{
#ifdef HEXER_HAVE_CAIRO
    p -= m_grid_p->origin();
    cairo_arc(m_cairo_p, p.m_x, p.m_y, 1.0, 0.0, 2 * M_PI);
    cairo_stroke(m_cairo_p);
#endif
}

} //namespace hexer
