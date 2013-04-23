/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Andrew Bell andrew.bell.ia at gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#ifndef INCLUDED_PSHAPE_SEGMENT_HPP
#define INCLUDED_PSHAPE_SEGMENT_HPP

#include <iostream>
#include <stdlib.h>
#include <vector>

#include "Mathpair.hpp"
#include "export.hpp"

namespace hexer
{

class Hexagon;
class HexGrid;

class HEXER_DLL Segment
{
public:
    Segment() : m_hex(NULL), m_side(0)
        {}

    Segment(Hexagon *h, int side) : m_hex(h), m_side(side)
        {}

    Hexagon *hex()
        { return m_hex; }

    int side()
        { return m_side; }

    bool horizontal()
        { return (m_side == 0 || m_side == 3); }

    void normalize(HexGrid *grid);
    bool possibleRoot(HexGrid *grid);

    Segment rightAntiClockwise(HexGrid *grid);
    Segment leftAntiClockwise(HexGrid *grid);
    Segment rightClockwise(HexGrid *grid);
    Segment leftClockwise(HexGrid *grid);
    Point startPos(HexGrid *grid) const;
    Point endPos(HexGrid *grid) const;

    friend bool operator == (const Segment& s1, const Segment &s2);
    friend bool operator != (const Segment& s1, const Segment &s2);
    friend std::ostream& operator << (std::ostream& os, const Segment &s);

private:
    Point pos(HexGrid *grid, const Point& offset) const;

    /// Hexagon who's side is the segment.
    Hexagon *m_hex;
    /// Which side of the hexagon.
    int m_side;

};

} // namespace

#endif // file guard
