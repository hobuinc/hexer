/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Andrew Bell andrew.bell.ia at gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#pragma once

#include <vector>
#include <ostream>

#include "export.hpp"
#include "HexGrid.hpp"

namespace hexer
{

class HEXER_DLL HexInfo
{
public:
    Point m_center;
    int m_density;
};

class HEXER_DLL HexIter
{
public:
    HexIter(HexGrid::HexMap::iterator iter, HexGrid *grid) :
        m_iter(iter), m_grid(grid)
    {
        advance();
    }

    HexIter& operator++()
    {
        m_iter++;
        advance();
        return *this;
    }

    HexInfo operator*()
    {
        HexInfo info;
        Hexagon& hex = m_iter->second;
        Point p;
        p.m_x = hex.x() * m_grid->width();
        p.m_y = hex.y() * m_grid->height();
        if (hex.xodd())
            p.m_y += (m_grid->height() / 2);
        info.m_center = p + m_grid->centerOffset(0);
        info.m_density = hex.count();
        return info;
    }

    bool operator == (const HexIter& iter)
        { return m_iter == iter.m_iter; }
    bool operator != (const HexIter& iter)
        { return m_iter != iter.m_iter; }

private:
    void advance()
    {
        while (m_iter != m_grid->m_hexes.end())
        {
            if (m_iter->second.count())
                break;
            m_iter++;
        }
    }

    HexGrid::HexMap::iterator m_iter;
    HexGrid *m_grid;
};

class HEXER_DLL GridInfo
{
public:
    GridInfo() : m_hexsize(-1), m_density(10)
        {}
    ~GridInfo()
        { delete m_grid; }

    double m_hexsize;
    int m_density;
    HexGrid *m_grid;

    std::vector<Path *> const& rootPaths() const
        { return m_grid->rootPaths(); }
    
    inline void addPoint(Point const& p)
        {
            m_grid->addPoint(p);
        };

    Point offset(int idx) const
        { return m_grid->centerOffset(idx); }

    double width() const
        { return m_grid->width(); }

    double height() const
        { return m_grid->height(); }

    HexIter begin()
        { return HexIter(m_grid->m_hexes.begin(), m_grid); }

    HexIter end()
        { return HexIter(m_grid->m_hexes.end(), m_grid); }
    
    void toWKT(std::ostream& strm) const 
        { m_grid->toWKT(strm); }
};

} // namespace hexer

