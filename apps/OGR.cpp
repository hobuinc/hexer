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

OGRWriter::OGRWriter(std::string const& filename, bool h3)
    : m_filename(filename)
    , m_isH3(h3)
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
    
    hFieldDefn = OGR_Fld_Create("COORDS", OFTString);
    if (OGR_L_CreateField(m_layer, hFieldDefn, TRUE) != OGRERR_NONE)
    {
        std::ostringstream oss;
        oss << "Could not create IJ field on layer with error '"
            << CPLGetLastErrorMsg() << "'";
        throw hexer_error(oss.str());
    }
    OGR_Fld_Destroy(hFieldDefn);

    if (m_isH3)
    {
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
        Segment s(id, i);
        Point p = grid->findPoint(s);
        OGR_G_AddPoint_2D(ring, p.m_x, p.m_y);
    }
    Segment s(id, 0);
    Point p = grid->findPoint(s);
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
    int counter(0);
    const std::unordered_map<HexId, int>& hexes = grid->getMap();
    for (auto iter = hexes.begin(); iter != hexes.end(); ++iter)
    {
        if (grid->isDense(iter->first))
        {
            HexId c = iter->first;
            std::ostringstream coords;
            coords << "(" << (int)c.i <<
                    ", " << (int)c.j << ")";
            OGRGeometryH polygon = collectHexagon(iter->first, grid);
            OGRFeatureH hFeature;

            hFeature = OGR_F_Create(OGR_L_GetLayerDefn(m_layer));
            OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "ID"),
                counter);
            OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "COUNT"),
                iter->second);
            OGR_F_SetFieldString( hFeature, OGR_F_GetFieldIndex(hFeature, "COORDS"),
                coords.str().c_str());
            processGeometry(m_layer, hFeature, polygon);
            counter++;
        }
    }
}

void OGRWriter::writeDensity(H3Grid *grid)
{
    int counter(0);
    const std::unordered_map<HexId, int>& hexes = grid->getMap();
    for (auto iter = hexes.begin(); iter != hexes.end(); ++iter)
    {
        if (grid->isDense(iter->first))
        {
            OGRGeometryH polygon = collectHexagon(iter->first, grid);
            OGRFeatureH hFeature;

            HexId c = iter->first;
            std::ostringstream coords;
            coords << "(" << (int)c.i <<
                    ", " << (int)c.j << ")";

            H3Index idx = grid->ij2h3(iter->first);
            char id[17];
            char* id_ptr = id;

            if (h3ToString(idx, id_ptr, 17) != E_SUCCESS)
                throw hexer_error("could not convert H3 ID to string!");

            hFeature = OGR_F_Create(OGR_L_GetLayerDefn(m_layer));
            OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "ID"),
                counter);
            OGR_F_SetFieldInteger( hFeature, OGR_F_GetFieldIndex(hFeature, "COUNT"),
                iter->second);
            OGR_F_SetFieldInteger64( hFeature, OGR_F_GetFieldIndex(hFeature, "H3_ID"),
                idx);
            OGR_F_SetFieldString( hFeature, OGR_F_GetFieldIndex(hFeature, "H3_STRING"),
                id_ptr);
            OGR_F_SetFieldString( hFeature, OGR_F_GetFieldIndex(hFeature, "COORDS"),
                coords.str().c_str());
            processGeometry(m_layer, hFeature, polygon);
            counter++;
        }
    }
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

} // namespace hexer
