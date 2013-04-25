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
#include <boost/filesystem.hpp>

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

    // OGRRegisterAll();
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
	
    // OGRCleanupAll();
#endif

}

} // reader

namespace writer
{

OGR::OGR(std::string const& filename) 
    : m_filename(filename)
	, m_ds(0)
	, m_layer(0)
{
// #ifdef HEXER_HAVE_GDAL
//     OGRRegisterAll();
// #endif
    createLayer();
}

void OGR::createLayer()
{
    using namespace boost::filesystem;
    
    OGRSFDriverH driver = OGRGetDriverByName("ESRI Shapefile");
    if (driver == NULL)
    {
        throw hexer_error("OGR Driver was null!");
    }
    
    m_ds = OGR_Dr_CreateDataSource(driver, m_filename.c_str(), NULL);
    if (m_ds == NULL)
    {
        throw hexer_error("Data source creation was null!");
    }
    
    path p(m_filename);
    std::string basename;
    path t = p;
    for (; !t.extension().empty(); t = t.stem())
    {
        if (t.stem().extension().empty())
        {
            basename = t.stem().string();
        }
    }
    
    std::cout << "basename: " << basename << std::endl;
    m_layer = OGR_DS_CreateLayer(m_ds, basename.c_str(), NULL, wkbMultiPolygon, NULL);
    if (m_layer == NULL)
    {
        throw hexer_error("Layer creation was null!");
    }
    
    OGRFieldDefnH hFieldDefn;
    hFieldDefn = OGR_Fld_Create("ID", OFTInteger);
    if (OGR_L_CreateField(m_layer, hFieldDefn, TRUE) != OGRERR_NONE)
    {
        throw hexer_error("Could not create ID field on layer!");
    }
    
    OGR_Fld_Destroy(hFieldDefn);
}

void OGR::writeWKT(std::string const& wkt) const
{
    OGRFeatureH hFeature;
    
    hFeature = OGR_F_Create(OGR_L_GetLayerDefn(m_layer));
    OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "ID"), 1);
    OGRGeometryH hPoly;
    
    const char* text = wkt.c_str();
    if( OGR_G_CreateFromWkt((char**)&text, NULL, &hPoly) != OGRERR_NONE )
    {
        throw hexer_error("Unable to create geometry from WKT!");
    }

    OGR_F_SetGeometry(hFeature, hPoly);
    OGR_G_DestroyGeometry(hPoly);

    if( OGR_L_CreateFeature( m_layer, hFeature ) != OGRERR_NONE )
    {
        throw hexer_error("Unable to create feature for WKT!");
    }
    
    OGR_F_Destroy( hFeature );
    
    
}

OGR::~OGR()
{
    OGR_DS_Destroy(m_ds);
// #ifdef HEXER_HAVE_GDAL
//     OGRCleanupAll();
// #endif

}

} // writer


} //namespace
