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

#include <functional>
#include <algorithm>
#include <iostream>
#include <sstream>

#include <hexer/BaseGrid.hpp>
/**
#include <hexer/HexIter.hpp>
#include <hexer/H3grid.hpp>
#include <hexer/H3Path.hpp>
**/

#include <h3/include/h3api.h>

using namespace std;

namespace hexer
{

OGRReader::OGRReader(std::string filename)
    : m_filename(filename)
    , m_index(0)
	, m_ds(0)
	, m_layer(0)
	, m_current_feature(0)
	, m_current_geometry(0)
{
    reader = std::bind(&OGRReader::read, std::placeholders::_1, std::placeholders::_2, this);
}

OGRReader::~OGRReader()
{
    if (m_current_feature)
        OGR_F_Destroy(m_current_feature);

    if (m_ds)
        OGR_DS_Destroy(m_ds);
}


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

OGRWriter::OGRWriter(std::string const& filename)
    : m_filename(filename)
	, m_ds(0)
	, m_layer(0)
{
    createLayer(getBasename(filename));
}

void OGRWriter::createLayer(std::string const& basename)
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

void OGRWriter::writeBoundary(BaseGrid *grid)
{
    OGRGeometryH multi = OGR_G_CreateGeometry(wkbMultiPolygon);

    const std::vector<Path *> paths = grid->rootPaths();
    for (const Path *path : paths)
    {
        OGRGeometryH polygon = OGR_G_CreateGeometry(wkbPolygon);
        collectPath(path, polygon);

        if( OGR_G_AddGeometryDirectly(multi, polygon) != OGRERR_NONE )
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

void OGRWriter::collectPath(const Path *path, OGRGeometryH polygon)
{
    OGRGeometryH ring = OGR_G_CreateGeometry(wkbLinearRing);

    for (const Point& p : path->points())
        OGR_G_AddPoint_2D(ring, p.m_x, p.m_y);

    if( OGR_G_AddGeometryDirectly(polygon, ring) != OGRERR_NONE )
    {
        std::ostringstream oss;
        oss << "Unable to add geometry with error '" <<
            CPLGetLastErrorMsg() << "'";
        throw hexer_error(oss.str());
    }

    for (const Path *path : path->subPaths())
        collectPath(path, polygon);
}

OGRGeometryH OGRWriter::collectHexagon(HexId const& id, BaseGrid *grid)
{
    OGRGeometryH ring = OGR_G_CreateGeometry(wkbLinearRing);

    for (int i = 0; i <= 5; ++i) {
        Point p = grid->findPoint(Segment(id, i));
        OGR_G_AddPoint_2D(ring, p.m_x, p.m_y);
    }
    Point p = grid->findPoint(Segment(id, 0));
    OGR_G_AddPoint_2D(ring, p.m_x, p.m_y);

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


void OGRWriter::writeDensity(HexGrid *grid)
{
    /**ABELL
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
    **/
}

void OGRWriter::processGeometry(OGRLayerH layer, OGRFeatureH feature, OGRGeometryH polygon)
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

OGRWriter::~OGRWriter()
{
    OGR_DS_Destroy(m_ds);
}

/**
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

    hFieldDefn = OGR_Fld_Create("H3_STRING", OFTString);
    if (OGR_L_CreateField(m_layer, hFieldDefn, TRUE) != OGRERR_NONE)
    {
        std::ostringstream oss;
        oss << "Could not create H3_STRING field on layer with error '"
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

    //OGRSpatialReferenceH oSRS = OSRNewSpatialReference( NULL );
    //std::string crs{"WGS84"};
    //if (OSRSetWellKnownGeogCS(oSRS, crs.c_str()) != OGRERR_NONE)
    //    throw hexer_error("Could not set crs to WGS84!");
    //OGR_G_AssignSpatialReference(ring, oSRS);
    //OSRDestroySpatialReference(oSRS);

    for (int i = 0; i <= b.numVerts - 1; ++i) {
        LatLng ll = b.verts[i];
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
    std::map<CoordIJ, int> hex_map = grid->getMap();

    int counter(0);
    for (auto iter = hex_map.begin(); iter != hex_map.end(); ++iter) {
        H3Index idx = grid->ij2h3(iter->first);
        char id[17];
        char* id_ptr = id;
        if (h3ToString(idx, id_ptr, 17) != E_SUCCESS)
            throw hexer_error("could not convert H3 ID to string!");

        CellBoundary bounds;
        if (cellToBoundary(idx, &bounds) != E_SUCCESS)
            throw hexer_error("Unable to collect H3 boundary!");

        OGRGeometryH polygon = collectH3(bounds);
        OGRFeatureH hFeature;
        hFeature = OGR_F_Create(OGR_L_GetLayerDefn(m_layer));
        OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "ID"),
            counter);
        OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "COUNT"),
            iter->second);
        OGR_F_SetFieldInteger64( hFeature, OGR_F_GetFieldIndex(hFeature, "H3_ID"),
            idx);
        OGR_F_SetFieldString( hFeature, OGR_F_GetFieldIndex(hFeature, "H3_STRING"),
            id_ptr);

        processGeometry(m_layer, hFeature, polygon);
        counter++;
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
    for (Point& p : pts)
        OGR_G_AddPoint_2D(ring, p.m_x, p.m_y);

    OGR_G_AddPoint_2D(ring, pts[0].m_x, pts[0].m_y);

    if( OGR_G_AddGeometryDirectly(polygon, ring) != OGRERR_NONE )
    {
        std::ostringstream oss;
        oss << "Unable to add geometry with error '" <<
            CPLGetLastErrorMsg() << "'";
        throw hexer_error(oss.str());
    }

    vector<H3Path *> paths = path->subPaths();
    for (int i = 0; i != paths.size(); ++i)
    {
        H3Path* p = paths[i];
        collectPath(p, polygon);
    }
}
OGR::~OGR()
{
    OGR_DS_Destroy(m_ds);
}

} // namespace h3
**/

} // namespace hexer
