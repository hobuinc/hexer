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
#include "lasfile.hpp"

#include <fstream>
#include <sstream>


using namespace std;

bool readlas(double& x, double& y)
{

    static int i(0);
    static bool done = false;
    static las_file las;

    if (done)
    {
        return false;
    }
    if ( !las.is_open() )
    {
        las.open("serpent_mound.las");
    }
    
    x = las.getX(i);
    y = las.getY(i);

    i++;
    
    if (i == las.points_count())
    {
        las.close();
        done = true;
    }
    
    return true;
}

// Read from simple file of doubles
bool readpoint(double& x, double& y)
{
    static ifstream in;
    static bool done = false;
    static int i = 0;

    if (done)
    {
        return false;
    }
    if ( !in.is_open() )
    {
        in.open("fastpoints.small");
    }
    if (in.eof() || i == 500000)
    {
        in.close();
        done = true;
        return false;
    }
    in.read((char *)&x, sizeof(x));
    in.read((char *)&y, sizeof(y));
    i++;
    return true;
}

bool readHex(int& x, int& y)
{
    static int pos = 0;

    static int coords[] = {
        0, 0,
        1, 0,
        2, 1,
        3, 1,
        4, 2,
        4, 3,
        4, 4,
        3, 4,
        2, 5,
        1, 5,
        0, 5,
        -1, 4,
        -2, 4,
        -3, 3,
        -3, 2,
        -3, 1,
        -2, 1,
        -1, 0,
        -1, 2,
        0, 2,
        1, 3,
        2, 3
    };

    static int num_coords = sizeof(coords) / (sizeof(coords[0]));
    
    if (pos + 1 < num_coords) {
        x = coords[pos++];
        y = coords[pos++];
        return true;
    }
    return false;
}


std::string indent(int l)
{
    std::string tabs;

    tabs.append(l * 2, ' ');
    return tabs;
}

void dumpPath(hexer::Path *p)
{
    using namespace hexer;

    static int level = 0;
    Orientation o = p->orientation();
    std::string ostring = ((o == CLOCKWISE) ? "CLOCKWISE" : "ANTICLOCKWISE");
    indent(level);
    cerr << indent(level) << "Path length = " << p->pathLength() << "!\n";
    cerr << indent(level) << "Orientation = " << ostring << "!\n";
    vector<Path *> paths = p->subPaths();
    level++;
    for (int pi = 0; pi != paths.size(); ++pi)
    {
        dumpPath(paths[pi]);
    }
    level--;
}


std::ostream& dumpBoundary(std::ostream& strm, hexer::Path *p)
{
    using namespace hexer;

    static int level = 0;
    Orientation o = p->orientation();
    indent(level);
    vector<Point> points = p->points();

    strm << "(";
    vector<Point>::const_iterator i;
    bool bFirst(true);
    for (i = points.begin(); i != points.end(); ++i)
    {
        if (!bFirst)
        {
            strm << ", ";
        } else
        {
            bFirst = false;
        }

        strm << i->m_x << " " << i->m_y;
    }
    strm << ")";
    

    vector<Path *> paths = p->subPaths();
    

    if (paths.size())
    {
        // bFirst = true;
        for (int pi = 0; pi != paths.size(); ++pi)
        {
            Path* p = paths[pi];
            // if (!bFirst)
            // {
                strm <<",";
                    // }
            dumpBoundary(strm, p);
        }
    }
    
    return strm;
}

void hextest()
{
    using namespace hexer;

    vector<GridInfo *> infos;
    GridInfo *gi = new GridInfo;

    gi->m_hexsize = 10;
    gi->m_density = 10;
    infos.push_back(gi);

    processHexes(infos, readHex);

    // Dump hexes.
    for (HexIter iter = gi->begin(); iter != gi->end(); ++iter)
    {
        HexInfo hi = *iter;
        cerr << "Density/X/Y = " << hi.m_density << "/" <<
            hi.m_center.m_x << "/" << hi.m_center.m_y << "!\n";
    }

    delete gi;
}

void pathtest()
{
    using namespace hexer;

    vector<GridInfo *> infos;
    GridInfo *gi = new GridInfo;

    infos.push_back(gi);
    process(infos, readlas);
    for (int pi = 0; pi < gi->rootPaths().size(); ++pi)
    {
        Path *p = gi->rootPaths()[pi];
        dumpPath(p);
    }

    delete gi;
}

void boundarytest()
{
    using namespace hexer;

    vector<GridInfo *> infos;
    GridInfo *gi = new GridInfo;

    infos.push_back(gi);
    process(infos, readlas);
    

    std::ostringstream multi;
    multi.setf(std::ios::fixed);
    multi.precision(8);
    
    multi << "MULTIPOLYGON (";
    
    bool bFirst(true);
    for (int pi = 0; pi < gi->rootPaths().size(); ++pi)
    {
        Path *p = gi->rootPaths()[pi];

        if (!bFirst)
        {
            multi << ",";
        } else {
            bFirst = false;
        }
            
        multi << "(";
        std::ostringstream poly;
        poly.setf(std::ios::fixed);
        poly.precision(8);
        dumpBoundary(poly, p);
        
        multi << poly.str();
        multi << ")";
        
    }
    multi << ")";
    
    std::cout << multi.str() << std::endl;

    delete gi;
}

int main()
{
//    hextest();
    // pathtest();
    boundarytest();
    return 0;
}

