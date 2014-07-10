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

namespace hexer
{

class HEXER_DLL HexInfo
{
public:
    Point m_center;
    Coord m_pos;
    int m_density;

    int density() const
        { return m_density; }
    int xgrid() const
        { return m_pos.m_x; }
    int ygrid() const
        { return m_pos.m_y; }
    double x() const
        { return m_center.m_x; }
    double y() const
        { return m_center.m_y; }
};

} // namespace hexer

