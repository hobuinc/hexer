/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Andrew Bell andrew.bell.ia at gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/


#include <iostream>

#include <hexer/OGR.hpp>
#include <hexer/HexGrid.hpp>

#include <boost/program_options.hpp>

using namespace std;



namespace hexer
{


OGR::OGR( HexGrid *grid_p, 
            std::string const& filename) 
    : m_grid_p(grid_p)
    , m_filename(filename)
{

#ifdef HEXER_HAVE_GDAL

    OGRRegisterAll();
#endif
    
}

OGR::~OGR()
{
#ifdef HEXER_HAVE_GDAL

#endif

}

void OGR::drawHexagon(Hexagon *hex_p, bool fill)
{
#ifdef HEXER_HAVE_GDAL
  
#endif    
}

void OGR::drawSegment(Segment s)
{
#ifdef HEXER_HAVE_GDAL

#endif    
}

void OGR::drawPoint(hexer::Point p)
{
#ifdef HEXER_HAVE_GDAL
#endif
}


} //namespace
