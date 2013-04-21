/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Andrew Bell andrew.bell.ia at gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#include <hexer/Path.hpp>

using namespace std;

namespace hexer
{

Point Path::getPoint(int pointnum) const
{
    pointnum = (m_orientation == ANTICLOCKWISE) ?
        m_segs.size() - pointnum - 1 : pointnum;
    return m_segs[pointnum].startPos(m_grid);
}

vector<Point> Path::points() const
{
    vector<Point> points;
    if (m_orientation == CLOCKWISE)
    {
        for (size_t i = 0; i < m_segs.size(); ++i)
        {
            points.push_back(m_segs[i].startPos(m_grid));
        }
        points.push_back(m_segs[0].startPos(m_grid));
    }
    else
    {
        // Note that i will wrap to max of size_t when decrementing 0.
        for (size_t i = m_segs.size() - 1; i < m_segs.size(); --i)
        {
            points.push_back(m_segs[i].startPos(m_grid));
        }
        points.push_back(m_segs[m_segs.size()-1].startPos(m_grid));

    }
    return points;
}

void Path::toWKT( std::ostream& output) const
{
    vector<Point> pts = points();

    output << "(";
    vector<Point>::const_iterator i;
    bool bFirst(true);
    for (i = pts.begin(); i != pts.end(); ++i)
    {
        if (!bFirst)
        {
            output << ", ";
        } else
        {
            bFirst = false;
        }

        output << i->m_x << " " << i->m_y;
    }
    output << ")";


    vector<Path *> paths = subPaths();
    for (int pi = 0; pi != paths.size(); ++pi)
    {
        Path* p = paths[pi];
        output <<",";
        p->toWKT(output);
    }
    
}

} // namespace
