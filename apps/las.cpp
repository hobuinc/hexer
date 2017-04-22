/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Howard Butler, hobu.inc@gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#include "las.hpp"
#include <functional>

LAS::LAS(std::string filename) 
    : m_filename(filename)
    , m_index(0) 
{
    reader = std::bind(&LAS::read, std::placeholders::_1, std::placeholders::_2, this);
};
