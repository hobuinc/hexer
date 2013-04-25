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
#include <hexer/GridInfo.hpp>

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
    
    m_layer = OGR_DS_CreateLayer(m_ds, basename.c_str(), NULL, wkbMultiPolygon, NULL);
    if (m_layer == NULL)
    {
        throw hexer_error("Layer creation was null!");
    }
    
    OGRFieldDefnH hFieldDefn;
    hFieldDefn = OGR_Fld_Create("ID", OFTInteger);
    if (OGR_L_CreateField(m_layer, hFieldDefn, TRUE) != OGRERR_NONE)
    {
        std::ostringstream oss;
        oss << "Could not create ID field on layer with error '" 
            << CPLGetLastErrorMsg() << "'";
        throw hexer_error(oss.str());
    }
    OGR_Fld_Destroy(hFieldDefn);

    hFieldDefn = OGR_Fld_Create("COUNT", OFTInteger);
    if (OGR_L_CreateField(m_layer, hFieldDefn, TRUE) != OGRERR_NONE)
    {
        std::ostringstream oss;
        oss << "Could not create COUNT field on layer with error '" 
            << CPLGetLastErrorMsg() << "'";
        throw hexer_error(oss.str());
    }
    OGR_Fld_Destroy(hFieldDefn);


}

void OGR::writeBoundary(std::vector<GridInfo*> const& infos)
{
    typedef std::vector<GridInfo*>::const_iterator it;
    int counter(0);
    for (it i = infos.begin(); i != infos.end(); ++i)
    {
        OGRGeometryH multi = OGR_G_CreateGeometry(wkbMultiPolygon);

    	typedef std::vector<hexer::Path*>::size_type st;
        for (st pi = 0; pi < (*i)->rootPaths().size(); ++pi)
        {
            OGRGeometryH polygon = OGR_G_CreateGeometry(wkbPolygon);
            Path *p = (*i)->rootPaths()[pi];
            collectPath(p, polygon);

            if( OGR_G_AddGeometryDirectly(multi, polygon ) != OGRERR_NONE )
            {
                std::ostringstream oss;
                oss << "Unable to add polygon to multipolygon with error '" 
                    << CPLGetLastErrorMsg() << "'";
                throw hexer_error(oss.str());
            }
        }
        
        OGRFeatureH hFeature;
    
        hFeature = OGR_F_Create(OGR_L_GetLayerDefn(m_layer));
        OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "ID"), counter);

        OGR_F_SetGeometry(hFeature, multi);
        OGR_G_DestroyGeometry(multi);

        if( OGR_L_CreateFeature( m_layer, hFeature ) != OGRERR_NONE )
        {
            std::ostringstream oss;
            oss << "Unable to create feature for multipolygon with error '" 
                << CPLGetLastErrorMsg() << "'";
            throw hexer_error(oss.str());
        }
        OGR_F_Destroy( hFeature );                
        counter++;
    }
}

void OGR::collectPath(Path* path, OGRGeometryH polygon)
{
    OGRGeometryH ring = OGR_G_CreateGeometry(wkbLinearRing);
    
    vector<Point> pts = path->points();

    vector<Point>::const_iterator i;
    for (i = pts.begin(); i != pts.end(); ++i)
    {
        OGR_G_AddPoint_2D(ring, i->m_x, i->m_y);
    }

    if( OGR_G_AddGeometryDirectly(polygon, ring) != OGRERR_NONE )
    {
        std::ostringstream oss;
        oss << "Unable to add geometry with error '" << CPLGetLastErrorMsg() << "'";
        throw hexer_error(oss.str());
    }

    vector<Path *> paths = path->subPaths();
    for (int pi = 0; pi != paths.size(); ++pi)
    {
        Path* p = paths[pi];
        collectPath(p, polygon);
    }
}

OGRGeometryH OGR::collectHexagon(HexInfo const& info, HexGrid const* grid)
{
    OGRGeometryH ring = OGR_G_CreateGeometry(wkbLinearRing);
	
    Point pos = info.m_center;
	pos += grid->origin();

	
    OGR_G_AddPoint_2D(ring, pos.m_x, pos.m_y);		
    for (int i = 1; i <= 5; ++i)
    {
        Point p = pos + grid->offset(i);
        OGR_G_AddPoint_2D(ring, p.m_x, p.m_y);		
    }
    OGR_G_AddPoint_2D(ring, pos.m_x, pos.m_y);		

    OGRGeometryH polygon = OGR_G_CreateGeometry(wkbPolygon);
    if( OGR_G_AddGeometryDirectly(polygon, ring ) != OGRERR_NONE )
    {
        std::ostringstream oss;
        oss << "Unable to add ring to polygon in collectHexagon '" 
            << CPLGetLastErrorMsg() << "'";
        throw hexer_error(oss.str());
    }
	
	return polygon;
	
}


void OGR::writeDensity(std::vector<GridInfo*> const& infos)
{
    typedef std::vector<GridInfo*>::const_iterator it;
    
    for (it gi = infos.begin(); gi != infos.end(); ++gi)
    {
		int counter(0);
	    for (HexIter iter = (*gi)->begin(); iter != (*gi)->end(); ++iter)
	    {

	        HexInfo hi = *iter;
			OGRGeometryH polygon = collectHexagon(hi, (*gi)->m_grid);
	        OGRFeatureH hFeature;
    
	        hFeature = OGR_F_Create(OGR_L_GetLayerDefn(m_layer));
	        OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "ID"), counter);
	        OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "COUNT"), hi.m_density);

	        OGR_F_SetGeometry(hFeature, polygon);
	        OGR_G_DestroyGeometry(polygon);

	        if( OGR_L_CreateFeature( m_layer, hFeature ) != OGRERR_NONE )
	        {
	            std::ostringstream oss;
	            oss << "Unable to create feature for multipolygon with error '" 
	                << CPLGetLastErrorMsg() << "'";
	            throw hexer_error(oss.str());
	        }
	        OGR_F_Destroy( hFeature );                
	        counter++;
	    }
    }
}


OGR::~OGR()
{
    OGR_DS_Destroy(m_ds);

}

} // writer


} //namespace
