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
#pragma once

#include <hexer/hexer.hpp>
#include <hexer/hexer_defines.h>

#include <string>

#ifdef HEXER_HAVE_CAIRO
#include <cairo/cairo.h>
#include <cairo/cairo-svg.h>
#endif

#include "Mathpair.hpp"
#include "export.hpp"

namespace hexer
{

class HexGrid;
class Hexagon;
class Segment;

struct HEXER_DLL Color
{
    Color(double red, double blue, double green) :
        m_red(red), m_blue(blue), m_green(green)
    {}

    double m_red;
    double m_blue;
    double m_green;
};

class HEXER_DLL Draw
{
public:
    Draw(HexGrid *grid_p, std::string const& filename);
    ~Draw();

    void drawHexagon(Hexagon *hex_p, bool fill = false);
    void drawSegment(Segment s, Color c = Color(0, 0, 1));
    void drawPoint(Point p);

private:
    HexGrid *m_grid_p;
    std::string m_filename;

#ifdef HEXER_HAVE_CAIRO
    cairo_surface_t *m_surface_p;
    cairo_t *m_cairo_p;
#endif

};

} // namespace

