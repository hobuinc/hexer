/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Howard Butler, hobu.inc@gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/



#include <hexer/GridInfo.hpp>

namespace hexer
{
    
void GridInfo::toWKT(std::ostream& output) const
{

    output << "MULTIPOLYGON (";
    
    bool bFirst(true);
    for (int pi = 0; pi < rootPaths().size(); ++pi)
    {
        Path *p = rootPaths()[pi];

        if (!bFirst)
        {
            output << ",";
        } else {
            bFirst = false;
        }
            
        output << "(";
        p->toWKT(output);
        
        output << ")";
        
    }
    output << ")";
    
}




} //namespace
