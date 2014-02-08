/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Andrew Bell andrew.bell.ia at gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#include <hexer/Processor.hpp>

#include <math.h>
#include <sstream>
#include <string>
#include <vector>

#ifdef HEXER_HAVE_GDAL
#include "gdal.h"
#endif

#include <hexer/GridInfo.hpp>
#include <hexer/Mathpair.hpp>

namespace hexer
{

    double distance(const Point& p1, const Point& p2)
    {
        double xdist = p2.m_x - p1.m_x;
        double ydist = p2.m_y - p1.m_y;
        return sqrt(xdist * xdist + ydist * ydist);
    }

    // Compute hex size based on distance between consecutive points and density.
    // The probably needs some work based on more data.
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


void process(const std::vector<GridInfo *>& infos, PointReader reader)
{
    std::vector<Point> samples;

    int cnt = 0;
    double x, y;
    void* context;

    // Max number of points to read to determine grid spacing.
    const int SAMPLE_COUNT = 5000;
    
    while (reader(x, y, context) && (cnt < SAMPLE_COUNT))
    {
        samples.push_back(Point(x,y));
        cnt++;
    }

    for (size_t gi = 0; gi < infos.size(); ++gi)
    {
        GridInfo *info = infos[gi];
        double hexsize = computeHexSize(samples, info->m_density);
        hexsize = (info->m_hexsize < 0) ?
            (-hexsize * info->m_hexsize) : info->m_hexsize;
        HexGrid *grid = new HexGrid(hexsize, info->m_density);
        info->m_grid = grid;

        for (std::vector<Point>::size_type i = 0; i < samples.size(); ++i)
        {
            grid->addPoint(samples[i]);
        }
        while (reader(x, y, context))
        {
            grid->addPoint(Point(x, y));
        }
        grid->findShapes();
        grid->findParentPaths();
    }
}

void processHexes(const std::vector<GridInfo *>& infos, HexReader reader)
{
    int cnt = 0;
    int x, y;
    void* ctx;

    for (size_t gi = 0; gi < infos.size(); ++gi)
    {
        GridInfo *info = infos[gi];
        assert(info->m_hexsize > 0);
        assert(info->m_density > 0);
        HexGrid *grid = new HexGrid(info->m_hexsize, info->m_density);
        info->m_grid = grid;
        
        while (reader(x, y, ctx))
        {
            grid->addDenseHexagon(x, y);
        }
        grid->findShapes();
        grid->findParentPaths();
    }
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
