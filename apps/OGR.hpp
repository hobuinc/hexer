/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Howard Butler, hobu.inc@gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/
#pragma once

#include <hexer/hexer.hpp>
#include <hexer/Path.hpp>
#include <hexer/Processor.hpp>
#include <hexer/hexer_defines.h>
#include <hexer/Mathpair.hpp>

#include "ogr_api.h"
#include "ogr_srs_api.h"
#include "gdal.h"

namespace hexer
{

class HexInfo;

class OGRReader
{
public:

    inline size_t const& getIndex() const
    {
        return m_index;
    }

    inline void setIndex(size_t v)
    {
        m_index = v;
    }

    inline size_t count() const
    {
        return static_cast<size_t>(OGR_L_GetFeatureCount(m_layer, 1));
    }

    inline bool open()
    {
		m_ds = OGROpen(m_filename.c_str(), 0, 0);
		m_layer = OGR_DS_GetLayer(m_ds, 0);
        return true;
    }

    inline bool close()
    {
		OGR_DS_Destroy(m_ds);
		m_ds = 0;
        return true;
    }

    inline double getX()
    {
		return OGR_G_GetX(m_current_geometry, 0);
    }

    inline double getY()
    {
		return OGR_G_GetY(m_current_geometry, 0);
    }

	bool getNextFeature()
	{
		if (m_current_feature)
			OGR_F_Destroy(m_current_feature);
		m_current_feature = OGR_L_GetFeature(m_layer, (long)m_index);
		if (!m_current_feature)
			return false;

		m_current_geometry = OGR_F_GetGeometryRef(m_current_feature);
		return true;
	}

    inline static bool read(double&x, double& y, void* ctx)
    {
		OGRReader* l = static_cast<OGRReader*>(ctx);

        if (l->getIndex() == l->count())
        {
            return false;
        }

		bool next = l->getNextFeature();
		if (next)
		{
	        x = l->getX();
	        y = l->getY();

	        l->setIndex(l->getIndex()+1);
		} else
			return false;

        return true;

    }

    hexer::PointReader reader;

private:
    std::string m_filename;
    size_t m_index;

    OGRDataSourceH m_ds;
	OGRLayerH m_layer;
	OGRFeatureH m_current_feature;
	OGRGeometryH m_current_geometry;


public:
    OGRReader(std::string filename);
	~OGRReader();
};

class OGRWriter
{


public:
    OGRWriter(std::string const& filename, bool h3 = false);
	~OGRWriter();

    void writeBoundary(BaseGrid& grid);
	void writeDensity(BaseGrid& grid);

private:
    std::string m_filename;
    bool m_isH3;

    OGRDataSourceH m_ds;
	OGRLayerH m_layer;

    void createLayer(std::string const& basename);
    void collectPath(const Path& path, OGRGeometryH polygon);
    void processGeometry(OGRLayerH layer, OGRFeatureH feature, OGRGeometryH polygon);
	OGRGeometryH collectHexagon(HexId const& id, BaseGrid& grid);

};

} // namespace hexer
