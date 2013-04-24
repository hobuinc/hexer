/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Howard Butler, hobu.inc@gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/


#include <iostream>

#include "OGR.hpp"
#include <hexer/HexGrid.hpp>

#include <boost/bind.hpp>

using namespace std;



namespace hexer
{

namespace reader
{

OGR::OGR(std::string filename) 
    : m_filename(filename)
    , m_index(0)
	, m_ds(0)
	, m_layer(0)
	, m_current_feature(0)
	, m_current_geometry(0)
{
#ifdef HEXER_HAVE_GDAL

    OGRRegisterAll();
	reader = boost::bind(&OGR::read, _1, _2, this);
#endif
    
}

OGR::~OGR()
{
#ifdef HEXER_HAVE_GDAL
	
	if (m_current_feature)
		OGR_F_Destroy(m_current_feature);
	
	if (m_ds)
		OGR_DS_Destroy(m_ds);
	
	OGRCleanupAll();
#endif

}

} // reader

namespace writer
{


} // writer


} //namespace
