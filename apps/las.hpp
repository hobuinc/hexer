/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Howard Butler, hobu.inc@gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#ifndef INCLUDED_HEXER_LAS_HPP
#define INCLUDED_HEXER_LAS_HPP

#include "lasfile.hpp"
#include <hexer/Processor.hpp>

class LAS
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
        return m_las.points_count();
    }

    inline bool open()
    {
        m_las.open(m_filename);
        return true;
    }
    
    inline bool close() 
    { 
        m_las.close(); 
        return true; 
    }
    
    inline double getX(size_t index)
    {
        return m_las.getX(index);
    }
    
    inline double getY(size_t index)
    {
        return m_las.getY(index);
    }
    
    inline static bool read(double&x, double& y, void* ctx)
    {

        LAS* l = static_cast<LAS*>(ctx);
        if (l->getIndex() == l->count())
        {
            return false;
        }

        x = l->getX(l->getIndex());
        y = l->getY(l->getIndex());

        l->setIndex(l->getIndex()+1);
        return true;
        
    }

    hexer::PointReader reader;

private:
    std::string m_filename;
    las_file m_las;
    size_t m_index;

public:
    LAS(std::string filename);
};

#endif