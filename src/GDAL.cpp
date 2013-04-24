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

#include <hexer/GDAL.hpp>

#include <hexer/Hexagon.hpp>
#include <hexer/HexGrid.hpp>
#include <hexer/Mathpair.hpp>
#include <hexer/Segment.hpp>
#include <hexer/hexer_defines.h>

using namespace std;



namespace hexer
{


GDAL::GDAL( HexGrid *grid_p, 
            std::string const& filename) 
    : m_grid_p(grid_p)
    , m_filename(filename)
{

#ifdef HEXER_HAVE_GDAL

#endif
    
}

GDAL::~GDAL()
{
#ifdef HEXER_HAVE_GDAL

#endif

}

void GDAL::drawHexagon(Hexagon *hex_p, bool fill)
{
#ifdef HEXER_HAVE_GDAL
  
#endif    
}

void GDAL::drawSegment(Segment s)
{
#ifdef HEXER_HAVE_GDAL

#endif    
}

void GDAL::drawPoint(hexer::Point p)
{
#ifdef HEXER_HAVE_GDAL
#endif
}


} //namespace
