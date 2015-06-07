/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Andrew Bell andrew.bell.ia at gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#include <assert.h>
#include <sstream>
#include <cmath>

#include <hexer/Processor.hpp>

#include <hexer/gitsha.h>
#include <hexer/HexGrid.hpp>

#ifdef HEXER_HAVE_GDAL
#include "gdal.h"
#endif

#include <hexer/Mathpair.hpp>

namespace hexer
{
    double distance(const Point& p1, const Point& p2)
    {
        double xdist = p2.m_x - p1.m_x;
        double ydist = p2.m_y - p1.m_y;
        return std::sqrt(xdist * xdist + ydist * ydist);
    }

    // Compute hex size based on distance between consecutive points and
    // density.  The probably needs some work based on more data.
    double computeHexSize(const std::vector<Point>& samples, int density)
    {
        double dist = 0;
        for (std::vector<Point>::size_type i = 0; i < samples.size() - 1; ++i)
        {
           Point p1 = samples[i];
           Point p2 = samples[i + 1];
           dist += distance(p1, p2);
        }
        return ((density * dist) / samples.size());
    }


void process(HexGrid *grid, PointReader reader)
{
    double x, y;
    void* context;

    while (reader(x, y, context))
        grid->addPoint(x, y);
    grid->findShapes();
    grid->findParentPaths();
}

void processHexes(HexGrid *grid, HexReader reader)
{
    int x, y;
    void* ctx;

    assert(grid->width() > 0);
    assert(grid->denseLimit() < 0);

    while (reader(x, y, ctx))
        grid->addDenseHexagon(x, y);
    grid->findShapes();
    grid->findParentPaths();
}

std::string GetFullVersion( void )
{
    std::ostringstream os;

    os << "hexer "
       << HEXER_VERSION_MAJOR << "."
       << HEXER_VERSION_MINOR << "."
       << HEXER_VERSION_PATCH;

    std::ostringstream revs;
    revs << g_GIT_SHA1;

    os << " at revision " << revs.str().substr(0, 6);

#ifdef HEXER_HAVE_GDAL
    os << " with GDAL " << GDALVersionInfo("RELEASE_NAME");
#endif

    return os.str();
}

} //namespace hexer
