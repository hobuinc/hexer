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

#include <stdint.h>

#include "export.hpp"
#include "Mathpair.hpp"

namespace hexer
{

class HEXER_DLL Hexagon
{
public:
    Hexagon(int x, int y) : m_x(x), m_y(y), m_count(0), m_dense(false),
        m_dense_neighbors(0)
        {}

    uint64_t key()
    {
        return key(m_x, m_y);
    }

    void increment()
       { m_count++; }

    static uint64_t key(int32_t x, int32_t y)
    {
        uint32_t ux = (uint32_t)x;
        uint32_t uy = (uint32_t)y;
        return (ux | ((uint64_t)uy << 32));
    }

    int x() const
        { return m_x; }

    int y() const
        { return m_y; }

    bool xodd() const
        { return (x() % 2 != 0); }

    bool xeven() const
        { return !xodd(); }

    void setDense()
        { m_dense = true; }

    bool dense() const
        { return m_dense; }

    int count() const
        { return m_count; } 

    void setCount(int count)
        { m_count = count; }

    void setDenseNeighbor(int dir)
        { m_dense_neighbors |= (1 << dir); }

    // We're surrounded by dense neighbors when the six low bits are set.
    bool surrounded() const
        { return (m_dense && (m_dense_neighbors == 0x3F)); }

    bool possibleRoot() const
    {
        const int TOP = (1 << 0);     // Top is side 0.
        return (m_dense && ((m_dense_neighbors & TOP) == 0) );
    }

    bool less(const Hexagon *h) const;
    bool yless(Hexagon *h) const;
    Coord neighborCoord(int dir) const;

private:
    int32_t m_x;
    int32_t m_y;
    int m_count;
    bool m_dense;
    int m_dense_neighbors;
};

class HexCompare
{
public:
    bool operator()(const Hexagon *h1, const Hexagon *h2)
        { return h1->less(h2); }
};

} // namespace

