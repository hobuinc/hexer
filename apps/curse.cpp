/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Andrew Bell andrew.bell.ia at gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <hexer/HexGrid.hpp>
#include <hexer/HexIter.hpp>
#include <hexer/Processor.hpp>
#include <hexer/Utils.hpp>
#include "ProgramArgs.hpp"
#include <lazperf/readers.hpp>
#include <lazperf/las.hpp>

#ifdef HEXER_HAVE_GDAL
#include "OGR.hpp"
#endif

std::string headline(hexer::Utils::screenWidth(), '-');



bool readHex(int& x, int& y, void* ctx)
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
    std::cerr << indent(level) << "Path length = " << p->pathLength() << "!\n";
    std::cerr << indent(level) << "Orientation = " << ostring << "!\n";
    std::vector<Path *> paths = p->subPaths();
    level++;
    for (int pi = 0; pi != paths.size(); ++pi)
    {
        dumpPath(paths[pi]);
    }
    level--;
}


void hextest(std::string filename)
{
    using namespace hexer;

    HexGrid grid(10, 10);

    // LAS l(filename);
    // l.open();
    // process(infos, l.reader);

    processHexes(&grid, readHex);

    // Dump hexes.
    for (HexIter iter = grid.hexBegin(); iter != grid.hexEnd(); ++iter)
    {
        HexInfo hi = *iter;
        std::cerr << "Density/X/Y = " << hi.m_density << "/" <<
            hi.m_center.m_x << "/" << hi.m_center.m_y << "!\n";
    }
}

void pathtest(std::string filename)
{
    using namespace hexer;

    HexGrid grid(10);

    std::ifstream file(filename, std::ios::binary);
    processLaz(&grid, file);

    std::vector<Path *> paths = grid.rootPaths();

    for (auto pi = paths.begin(); pi != paths.end(); ++pi)
        dumpPath(*pi);
}

enum FormatType
{
    Format_LAS = 0,
    Format_OGR = 1,
    Format_Unknown = 128

};

FormatType getDriver(std::string filename)
{
    auto idx = filename.find_last_of('.');
    if (idx == std::string::npos)
        return Format_LAS;
    else if (hexer::Utils::iequals(filename.substr(idx), ".LAS") || hexer::Utils::iequals(filename.substr(idx), ".LAZ"))
        return Format_LAS;
    else {
#ifdef HEXER_HAVE_GDAL

        return Format_OGR;
#endif
    }
}

void boundary(  std::string const& input,
                std::string const& output,
                double edge,
                int density)
{
    using namespace hexer;

    std::unique_ptr<HexGrid> grid;

    if (density == 0)
        density = 10;
    if (edge == 0.0)
        grid.reset(new HexGrid(density));
    else
        grid.reset(new HexGrid(edge, density));

    FormatType t = getDriver(input);
    if (t == Format_LAS)
    {
        std::ifstream file(input, std::ios::binary);
        processLaz(grid.get(), file);
    } else {
#ifdef HEXER_HAVE_GDAL
        reader::OGR o(input);
        o.open();
        process(grid.get(), o.reader);
#endif
    }

    if (output.empty() || hexer::Utils::iequals(output, "STDOUT"))
    {
        std::ostringstream multi;
        multi.setf(std::ios::fixed);
        multi.precision(8);

        grid->toWKT(multi);
        std::cout << multi.str() << std::endl;
    }
    else
    {
#ifdef HEXER_HAVE_GDAL
        writer::OGR o(output);
        o.writeBoundary(grid.get());
#endif
    }
}


void density(   std::string const& input,
                std::string const& output,
                double edge,
                int density)
{
    using namespace hexer;

    std::unique_ptr<HexGrid> grid;

    if (density == 0)
        density = 10;
    if (edge == 0.0) 
        grid.reset(new HexGrid(density));
    else
        grid.reset(new HexGrid(edge, density));

    FormatType t = getDriver(input);
    if (t == Format_LAS)
    {
        std::ifstream file(input, std::ios::binary);
        processLaz(grid.get(), file);
    } else {
#ifdef HEXER_HAVE_GDAL
        reader::OGR o(input);
        o.open();
        process(grid.get(), o.reader);
#endif
    }

#ifdef HEXER_HAVE_GDAL
    writer::OGR o(output);
    o.writeDensity(grid.get());
#endif
}


void OutputHelp( std::ostream & oss, hexer::ProgramArgs& args)
{
    oss << headline << std::endl;
    oss << "    curse (" << hexer::GetFullVersion() << ")" << std::endl;
    oss << headline << std::endl;

    args.dump2(oss, 2, 6, headline.size());

    oss <<"For more information, see the full documentation for hexer at:" << std::endl;

    oss << " http://github.com/hobu/hexer" << std::endl;
    oss << headline << std::endl;

}


int main(int argc, char* argv[])
{

#ifdef HEXER_HAVE_GDAL
    OGRRegisterAll();
#endif

    bool bVerbose(false);

    hexer::ProgramArgs args;

    std::string command;
    bool showHelp(false);
    bool showVersion(false);
    std::string input("");
    std::string output("");
    double edge(0.0);
    int count(0);

    args.add("command,c", "Command to run on points ('boundary' or 'density')", command, "boundary").setPositional();
    args.add("input,i", "Input point set to curse", input).setPositional();
    args.add("output,o", "Specify and OGR-compatible output filename to write boundary. stdout used if none specified.", output, "").setPositional();
    args.add("edge", "Edge distance of hexagon", edge, 0.0);
    args.add("count", "Number of points that must be in polygon for it to be positive space", count, 0);

    std::vector< std::string > argList;
    for (int i = 1; i < argc; ++i)
        argList.push_back(argv[i]);

    try
    {
        args.parseSimple(argList);
    }
    catch (hexer::arg_error const& e)
    {
        std::cout << "validation error: " << e.what() << std::endl;
        OutputHelp(std::cout, args);
        return -1;
    }


    if (showHelp)
    {
        OutputHelp(std::cout, args);
        return 1;
    }

    if (showVersion)
    {
        std::cout << hexer::GetFullVersion() << std::endl;
        return 0;
    }

    if (input.empty())
    {
        std::cerr << "Input not specified!\n";
        OutputHelp(std::cout, args);
        return 1;
    }


    try
    {
        if (hexer::Utils::iequals(command, "BOUNDARY"))
        {
            boundary(input, output, edge, count);
            return 0;
        }

        if (hexer::Utils::iequals(command, "DENSITY"))
        {
            density(input, output, edge, count);
            return 0;
        }

        if (hexer::Utils::iequals(command, "PATH"))
        {
            pathtest(input);
            return 0;
        }

        if (hexer::Utils::iequals(command, "HEX"))
        {
            hextest(input);
            return 0;
        }

        return 1;
    } catch (hexer::hexer_error const& e)
    {
        std::cout << "Curse failed with error: '" << e.what() << "'" << std::endl;
        return 1;
    }

#ifdef HEXER_HAVE_GDAL
    OGRCleanupAll();
#endif

}

