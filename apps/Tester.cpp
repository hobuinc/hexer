/**
#include <iomanip>
#include <vector>

#include "HexGrid.hpp"
#include "Mathpair.hpp"
**/

#include <fstream>

#include <hexer/Processor.hpp>
#include "lasfile.hpp"


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
    // in.read((char *)&x, sizeof(x));
    // in.read((char *)&y, sizeof(y));
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

void dumpPath(Pshape::Path *p)
{
    using namespace Pshape;

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

void hextest()
{
    using namespace Pshape;

    vector<GridInfo *> infos;
    GridInfo *gi = new GridInfo;

    gi->m_hexsize = 10;
    gi->m_density = 10;
    infos.push_back(gi);

    processHexes(infos, readHex);
    for (int pi = 0; pi < gi->rootPaths().size(); ++pi)
    {
        Path *p = gi->rootPaths()[pi];
        dumpPath(p);
    }

    // Dump hexes.
    for (HexIter iter = gi->begin(); iter != gi->end(); ++iter)
    {
        HexInfo hi = *iter;
        cerr << "Density/X/Y = " << hi.m_density << "/" <<
            hi.m_center.m_x << "/" << hi.m_center.m_y << "!\n";
    }

    delete gi;
}

void pointtest()
{
    using namespace Pshape;

    vector<GridInfo *> infos;
    GridInfo *gi = new GridInfo;

    infos.push_back(gi);
    process(infos, readlas);
    for (int pi = 0; pi < gi->rootPaths().size(); ++pi)
    {
        Path *p = gi->rootPaths()[pi];
        dumpPath(p);
    }

    // // Dump hexes.
    // for (HexIter iter = gi->begin(); iter != gi->end(); ++iter)
    // {
    //     HexInfo hi = *iter;
    //     cerr << "Density/X/Y = " << hi.m_density << "/" <<
    //         hi.m_center.m_x << "/" << hi.m_center.m_y << "!\n";
    // }

    delete gi;
}

int main()
{
//    hextest();
    pointtest();
    return 0;
}

