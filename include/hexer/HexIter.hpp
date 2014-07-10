/***************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Andrew Bell andrew.bell.ia at gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#pragma once

#include "export.hpp"
#include "HexGrid.hpp"
#include "HexInfo.hpp"

namespace hexer
{

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
        info.m_pos.m_x = hex.x();
        info.m_pos.m_y = hex.y();
        info.m_center.m_x = m_grid->width() * hex.x();
        info.m_center.m_y = m_grid->height() * hex.y();
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

} // namespace hexer

