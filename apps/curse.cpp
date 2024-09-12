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

#include <hexer/Processor.hpp>
#include <hexer/Utils.hpp>
#include "ProgramArgs.hpp"
#include <lazperf/readers.hpp>
#include <lazperf/las.hpp>
#include <hexer/BaseGrid.hpp>
#include <hexer/H3grid.hpp>
#include <hexer/HexGrid.hpp>


#include "OGR.hpp"

std::string headline(hexer::Utils::screenWidth(), '-');

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
    else
        return Format_OGR;
}

void runProcesses(  std::string const& command,
                    std::string const& input,
                    std::string const& output,
                    hexer::BaseGrid& grid)
{
    using namespace hexer;

    FormatType t = getDriver(input);
    if (t == Format_LAS) {
        std::ifstream file(input, std::ios::binary);
        processLaz(grid, file);
    }
    else {
        OGRReader o(input);
        o.open();
        process(grid, o.reader, o.count());
    }

    if (hexer::Utils::iequals(command, "BOUNDARY"))
    {
        if (output.empty() || hexer::Utils::iequals(output, "STDOUT")) {
            std::ostringstream multi;
            multi.setf(std::ios::fixed);
            multi.precision(8);

            grid.toWKT(multi);
            std::cout << multi.str() << std::endl;
        }
        else {
            OGRWriter o(output, true);
            o.writeBoundary(&grid);
        }
    }
    else if (hexer::Utils::iequals(command, "DENSITY")) 
    {
        OGRWriter o(output, true);
        o.writeDensity(&grid);
    }
}

void runHexer(  std::string const& command,
                std::string const& input,
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

    runProcesses(command, input, output, *grid);

}

void runH3(     std::string const& command,
                std::string const& input,
                std::string const& output,
                int res,
                int density)
{
    using namespace hexer;

    std::unique_ptr<H3Grid> grid;

    if (density == 0)
        density = 10;
    if (res > 15 || res < -1) {
        throw hexer_error("Input an H3 grid cell size between 0 and 15."
        "Info on H3 cell specifications can be found at https://h3geo.org/docs/core-library/restable");
    }
    else
        grid.reset(new H3Grid(res, density));

    runProcesses(command, input, output, *grid);

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

    OGRRegisterAll();

    bool bVerbose(false);

    hexer::ProgramArgs args;

    std::string command;
    bool showHelp(false);
    bool showVersion(false);
    std::string input;
    std::string output;
    double edge(0.0);
    int count(0);
    int res(-1);
    std::string grid;

    args.add("command,c", "Command to run on points ('boundary' or 'density')", command, "boundary").setPositional();
    args.add("input,i", "Input point set to curse", input).setPositional();
    args.add("output,o", "Specify and OGR-compatible output filename to write boundary. stdout used if none specified.", output, "").setPositional();
    args.add("edge", "Edge distance of hexagon", edge, 0.0);
    args.add("count", "Number of points that must be in polygon for it to be positive space", count, 0);
    args.add("resolution,r", "H3 grid resolution: 0 (coarsest) - 15 (finest)", res, -1);
    args.add("grid", "Grid type ('hexgrid' or 'h3'): proprietary HexGrid hexagons, or H3 indexed grid", grid, "hexgrid");

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
        if (hexer::Utils::iequals(command, "DENSITY") || hexer::Utils::iequals(command, "BOUNDARY"))
        {
            if (hexer::Utils::iequals(grid, "H3"))
            {
                runH3(command, input, output, res, count);
                return 0;
            }

            if (hexer::Utils::iequals(grid, "HEXGRID"))
            {
                runHexer(command, input, output, edge, count);
                return 0;
            }
        }
        else
        {
            std::cerr << "Command not accepted! Enter 'density' or 'boundary'\n";
            OutputHelp(std::cout, args);
            return 1;
        }


        return 1;
    } catch (hexer::hexer_error const& e)
    {
        std::cout << "Curse failed with error: '" << e.what() << "'" << std::endl;
        return 1;
    }

    OGRCleanupAll();

}

