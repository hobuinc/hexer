/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Andrew Bell andrew.bell.ia at gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#pragma once

#include <string>
#include <vector>

#include <functional>

#include <hexer/GridInfo.hpp>
#include <hexer/Path.hpp>
#include <hexer/export.hpp>

namespace hexer
{
    typedef std::function<bool(double&, double&, void*&)> PointReader;
    typedef std::function<bool(int&, int&, void*&)> HexReader;

    HEXER_DLL void process(const std::vector<GridInfo *>& infos, PointReader);
    HEXER_DLL void processHexes(const std::vector<GridInfo *>& infos,
        HexReader);
    
    HEXER_DLL double computeHexSize(const std::vector<Point>& samples,
        int density);

    HEXER_DLL std::string GetFullVersion( void );
} // namespace hexer

