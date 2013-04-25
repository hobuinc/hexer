/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Howard Butler, hobu.inc@gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#ifndef INCLUDED_PSHAPE_OGR_HPP
#define INCLUDED_PSHAPE_OGR_HPP

#include <hexer/hexer.hpp>
#include <hexer/hexer_defines.h>
#include <hexer/Processor.hpp>
#include <hexer/GridInfo.hpp>

#ifdef HEXER_HAVE_GDAL

#include "ogr_api.h"
#include "gdal.h"

#endif

#include "Mathpair.hpp"
#include "export.hpp"

namespace hexer
{

namespace reader
{

class HEXER_DLL OGR
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
		m_current_feature = OGR_L_GetFeature(m_layer, m_index);
		if (!m_current_feature)
			return false;
		
		m_current_geometry = OGR_F_GetGeometryRef(m_current_feature);
		return true;
	}

    inline static bool read(double&x, double& y, void* ctx)
    {
		OGR* l = static_cast<OGR*>(ctx);
		
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

#ifdef HEXER_HAVE_GDAL
    OGRDataSourceH m_ds;
	OGRLayerH m_layer;
	OGRFeatureH m_current_feature;
	OGRGeometryH m_current_geometry;
#endif


public:
    OGR(std::string filename);
	~OGR();
};

} // reader

namespace writer 
{


class HEXER_DLL OGR
{

    
public:
    OGR(std::string const& filename);
	~OGR();

    void writeBoundary(std::vector<GridInfo*> const& infos);

private:
    std::string m_filename;


#ifdef HEXER_HAVE_GDAL
    OGRDataSourceH m_ds;
	OGRLayerH m_layer;

#endif
    void createLayer();
    void collectGeometry(Path* path, OGRGeometryH polygon);

};

} // writer

} // namespace

#endif // file guard
