/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Howard Butler, hobu.inc@gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#include "OGR.hpp"

#include <iostream>
#include <algorithm>
#include <sstream>

#include <hexer/HexGrid.hpp>
#include <hexer/HexIter.hpp>
#include <hexer/H3grid.hpp>
#include <hexer/H3Path.hpp>

#include <h3/include/h3api.h>
#include <functional>

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
reader = std::bind(&OGR::read, std::placeholders::_1, std::placeholders::_2, this);

}

OGR::~OGR()
{
if (m_current_feature)
    OGR_F_Destroy(m_current_feature);

if (m_ds)
    OGR_DS_Destroy(m_ds);
}

} // reader

namespace writer
{


std::string getBasename(const std::string& path)
{
#ifdef _WIN32
    char sep('\\');
#else
    char sep('/');
#endif

    auto idx = path.find_last_of('.');
    std::string name("");
    if (idx == std::string::npos)
        name = path;
    else
        name = path.substr(0,idx);
    return {std::find_if(name.rbegin(), name.rend(),
                         [sep](char c) { return c == sep; }).base(),
            name.end()};
}

OGR::OGR(std::string const& filename)
    : m_filename(filename)
	, m_ds(0)
	, m_layer(0)
{
    createLayer(getBasename(filename));
}

void OGR::createLayer(std::string const& basename)
{
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

void OGR::writeBoundary(HexGrid *grid)
{
    OGRGeometryH multi = OGR_G_CreateGeometry(wkbMultiPolygon);

    const std::vector<hexer::Path *>& paths = grid->rootPaths();
    for (auto pi = paths.begin(); pi != paths.end(); ++pi)
    {
        OGRGeometryH polygon = OGR_G_CreateGeometry(wkbPolygon);
        collectPath(*pi, polygon);

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
    OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "ID"), 0);

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
        oss << "Unable to add geometry with error '" <<
            CPLGetLastErrorMsg() << "'";
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


void OGR::writeDensity(HexGrid *grid)
{
    int counter(0);
    for (HexIter iter = grid->hexBegin(); iter != grid->hexEnd(); ++iter)
    {

        HexInfo hi = *iter;
        OGRGeometryH polygon = collectHexagon(hi, grid);
        OGRFeatureH hFeature;

        hFeature = OGR_F_Create(OGR_L_GetLayerDefn(m_layer));
        OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "ID"),
            counter);
        OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "COUNT"),
            hi.m_density);
        processGeometry(m_layer, hFeature, polygon);
        counter++;
    }
}

void OGR::processGeometry(OGRLayerH layer, OGRFeatureH feature, OGRGeometryH polygon)
{
    OGR_F_SetGeometry(feature, polygon);
    OGR_G_DestroyGeometry(polygon);

    if( OGR_L_CreateFeature( layer, feature ) != OGRERR_NONE )
    {
        std::ostringstream oss;
        oss << "Unable to create feature for multipolygon with error '"
            << CPLGetLastErrorMsg() << "'";
        throw hexer_error(oss.str());
    }
    OGR_F_Destroy( feature ); 
}

OGR::~OGR()
{
    OGR_DS_Destroy(m_ds);
}

namespace h3
{

OGR::OGR(std::string const& filename)
    : m_filename(filename)
	, m_ds(0)
	, m_layer(0)
{
    createLayer(getBasename(filename));
}

void OGR::createLayer(std::string const& basename)
{
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

    hFieldDefn = OGR_Fld_Create("H3_ID", OFTInteger64);
    if (OGR_L_CreateField(m_layer, hFieldDefn, TRUE) != OGRERR_NONE)
    {
        std::ostringstream oss;
        oss << "Could not create H3_ID field on layer with error '"
            << CPLGetLastErrorMsg() << "'";
        throw hexer_error(oss.str());
    }
    OGR_Fld_Destroy(hFieldDefn);
    
    hFieldDefn = OGR_Fld_Create("IJ", OFTString);
    if (OGR_L_CreateField(m_layer, hFieldDefn, TRUE) != OGRERR_NONE)
    {
        std::ostringstream oss;
        oss << "Could not create IJ field on layer with error '"
            << CPLGetLastErrorMsg() << "'";
        throw hexer_error(oss.str());
    }
    OGR_Fld_Destroy(hFieldDefn);


}

void OGR::processGeometry(OGRLayerH layer, OGRFeatureH feature, OGRGeometryH polygon)
{
    OGR_F_SetGeometry(feature, polygon);
    OGR_G_DestroyGeometry(polygon);

    if( OGR_L_CreateFeature( layer, feature ) != OGRERR_NONE )
    {
        std::ostringstream oss;
        oss << "Unable to create feature for multipolygon with error '"
            << CPLGetLastErrorMsg() << "'";
        throw hexer_error(oss.str());
    }
    OGR_F_Destroy( feature ); 
}

OGRGeometryH OGR::collectH3(CellBoundary b)
{
    OGRGeometryH ring = OGR_G_CreateGeometry(wkbLinearRing);

    for (int i = 0; i <= b.numVerts - 1; ++i) {
        LatLng ll = b.verts[i];
        //std::cout << ll.lng << " " << ll.lat << std::endl;
        OGR_G_AddPoint_2D(ring, radsToDegs(ll.lng), radsToDegs(ll.lat));
    }
    OGR_G_AddPoint_2D(ring, radsToDegs(b.verts[0].lng), radsToDegs(b.verts[0].lat));

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

void OGR::writeDensity(H3Grid *grid)
{
    std::map<H3Index, int> hex_map = grid->getMap();
    std::vector<std::string> ij_arr = grid->getIJArr();
    int counter(0);
    for (auto iter = hex_map.begin(); iter != hex_map.end(); ++iter) {
        CellBoundary bounds;
        H3Error err = cellToBoundary(iter->first, &bounds);
        if (err == E_SUCCESS) {
            OGRGeometryH polygon = collectH3(bounds);
            OGRFeatureH hFeature;
            hFeature = OGR_F_Create(OGR_L_GetLayerDefn(m_layer));
            // H3 id: make new string field in createLayer
            OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "ID"),
                counter);
            OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "COUNT"),
                iter->second);
            OGR_F_SetFieldInteger64( hFeature, OGR_F_GetFieldIndex(hFeature, "H3_ID"),
                iter->first);
            OGR_F_SetFieldString( hFeature, OGR_F_GetFieldIndex(hFeature, "IJ"),
                ij_arr[counter].c_str());  
            processGeometry(m_layer, hFeature, polygon);
            counter++;
        }
        else
            throw hexer_error("Unable to collect H3 boundary!"); 
    }
}

void OGR::writeBoundary(H3Grid *grid)
{
    OGRGeometryH multi = OGR_G_CreateGeometry(wkbMultiPolygon);

    const std::vector<hexer::H3Path *> paths = grid->rootPaths();
    for (auto pi = paths.begin(); pi != paths.end(); ++pi)
    {
        OGRGeometryH polygon = OGR_G_CreateGeometry(wkbPolygon);
        collectPath(*pi, polygon);

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
    OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "ID"), 0);

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
}

void OGR::collectPath(H3Path* path, OGRGeometryH polygon)
{
    OGRGeometryH ring = OGR_G_CreateGeometry(wkbLinearRing);
    vector<Point> pts = path->getPoints();

    vector<Point>::const_iterator i;
    for (i = pts.begin(); i != pts.end(); ++i)
    {
        OGR_G_AddPoint_2D(ring, i->m_x, i->m_y);
    }
    OGR_G_AddPoint_2D(ring, pts[0].m_x, pts[0].m_y);

    if( OGR_G_AddGeometryDirectly(polygon, ring) != OGRERR_NONE )
    {
        std::ostringstream oss;
        oss << "Unable to add geometry with error '" <<
            CPLGetLastErrorMsg() << "'";
        throw hexer_error(oss.str());
    }

    vector<H3Path *> paths = path->subPaths();
    for (int pi = 0; pi != paths.size(); ++pi)
    {
        H3Path* p = paths[pi];
        collectPath(p, polygon);
    }
}
OGR::~OGR()
{
    OGR_DS_Destroy(m_ds);
}

} // namespace h3
} // namespace writer
} // namespace hexer
