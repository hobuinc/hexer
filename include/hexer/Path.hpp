/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Andrew Bell andrew.bell.ia at gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#ifndef INCLUDED_PSHAPE_PATH_HPP
#define INCLUDED_PSHAPE_PATH_HPP

#include <vector>
#include <ostream>

#include "Mathpair.hpp"
#include "Segment.hpp"

namespace hexer
{

enum Orientation
{
    CLOCKWISE,     // Outer
    ANTICLOCKWISE  // Hole
};

class HexGrid;

class HEXER_DLL Path
{
public:
    Path(HexGrid *m_grid, Orientation orient) :
        m_grid(m_grid), m_parent(NULL), m_orientation(orient)
    {}

    ~Path()
    {
        for (std::vector<Path*>::size_type i = 0; i < m_children.size(); ++i)
            delete m_children[i];
    }

    void push_back(const Segment& s)
        { m_segs.push_back(s); }
    Segment rootSegment()
        { return m_segs[0]; }
    Path *parent()
        { return m_parent; }
    void setParent(Path *p)
        { m_parent = p; }
    void addChild(Path *p)
        { m_children.push_back(p); }
    void finalize(Orientation o)
    {
        m_orientation = o;
        for (size_t i = 0; i < m_children.size(); ++i)
            m_children[i]->finalize(o == CLOCKWISE ? ANTICLOCKWISE : CLOCKWISE);
    }
    int pathLength() const
        { return m_segs.size(); }
    Point getPoint(int pointnum) const;
    Orientation orientation() const
        { return m_orientation; }
    std::vector<Point> points() const;
    std::vector<Path *> subPaths() const
        { return m_children; }
    void toWKT( std::ostream& output) const;

private:
    /// Grid that owns the path.
    HexGrid *m_grid;
    /// Parent path (NULL if root path)
    Path *m_parent;
    /// Children
    std::vector<Path *> m_children;
    /// Orientation of path AT EXTRACTION - segments are ALWAYS ordered
    /// clockwise.
    Orientation m_orientation;
    /// List of segments that make up the path.
    std::vector<Segment> m_segs;
};

} //namespace

#endif
