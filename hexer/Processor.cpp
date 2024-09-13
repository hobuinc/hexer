/******************************************************************************
 * Copyright (c) 2014, Hobu Inc. (howard@hobu.co)
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following
 * conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of the Howard Butler or Hobu, Inc.
 *       the names of its contributors may be
 *       used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 ****************************************************************************/

#include <assert.h>
#include <sstream>
#include <cmath>
#include <map>
#include <chrono>

#include <hexer/Processor.hpp>

#include <hexer/HexGrid.hpp>

#include <lazperf/readers.hpp>
#include <lazperf/las.hpp>

#include <h3/include/h3api.h>
#include <hexer/H3grid.hpp>

#include <hexer/Mathpair.hpp>

namespace hexer
{

void process(BaseGrid& grid, PointReader reader, int count)
{
    double x, y;
    void* context;

    grid.setSampleSize(std::min(count, 10000));

    while (reader(x, y, context))
    {
        grid.addXY(x, y);
    }
    grid.findShapes();
    grid.findParentPaths();
}

void processLaz(BaseGrid& grid, std::ifstream& file)
{

    lazperf::reader::generic_file l(file);

    size_t count = l.pointCount();

    lazperf::header14 h = l.header();
    uint16_t len = h.point_record_length;
    std::vector<char> buf(len, 0);
    char* buf_data = buf.data();

    grid.setSampleSize(std::min(int(count), 10000));

    for(size_t i = 0; i < count; i ++) {
        l.readPoint(buf_data);

        int32_t *pos = (int32_t *)buf_data;
        int32_t x_int = *pos;
        pos++;
        int32_t y_int = *pos;

        double x = (x_int * h.scale.x + h.offset.x);
        double y = (y_int * h.scale.y + h.offset.y);
        
        grid.addXY(x, y);
    }

    grid.findShapes();
    grid.findParentPaths();
}

void troubleshootH3(H3Grid& grid)
{
    grid.setHexes(
            {
                {5, 2}, {5, 3},
                {6, 2}, {6, 4},
                {7, 3}, {7, 4},
                {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5},
                {4, 0}, {4, 4}, {4, 6},
                {5, 0}, {5, 2}, {5, 3}, {5, 5}, {5, 7},
                {6, 0}, {6, 2}, {6, 4}, {6, 6}, {6, 8},
                {7, 1}, {7, 3}, {7, 4}, {7, 7}, {7, 8},
                {8, 2}, {8, 7}, {8, 8},
                {9, 3}, {9, 5}, {9, 7}, {9, 8},
                {10, 4}, {10, 8},
                {11, 5}, {11, 6}, {11, 7}, {11, 8},
            });
    LatLng location;
    location.lat = degsToRads(40.689167);
    location.lng = degsToRads(-74.044444);
    int resolution = 11;
    H3Index index;
    if(latLngToCell(&location, resolution, &index) != E_SUCCESS)
        std::cout<<"a"; 
    grid.setOrigin(index);
    grid.findShapes();
    grid.findParentPaths();
}
void troubleshootHexer(HexGrid& grid)
{
    grid.setHexes(
            {
                {0, 3}, {0, 4}, {0,5}, {0, 6},
                {1, 2}, {1, 6},
                {2, 2}, {2, 4}, {2, 5}, {2, 7},
                {3, 1}, {3, 3}, {3, 5}, {3, 7},
                {4, 1}, {4, 2}, {4, 4}, {4, 5}, {4, 8},
                {5, 0}, {5, 2}, {5, 6}, {5, 8},
                {6, 1}, {6, 3}, {6, 4}, {6, 8},
                {7, 1}, {7, 3}, {7, 4}, {7, 5}, {7, 7},
                {8, 2}, {8, 3}, {8, 4}, {8, 5}, {8, 6}, {8, 7}
            });
    grid.findShapes();
    grid.findParentPaths();
}
std::string GetFullVersion( void )
{
    std::ostringstream os;

    os << "hexer "
       << HEXER_VERSION_MAJOR << "."
       << HEXER_VERSION_MINOR << "."
       << HEXER_VERSION_PATCH;

    std::ostringstream revs;
    revs << hexerSha;

    os << " at revision " << revs.str().substr(0, 6);

    return os.str();
}

} //namespace hexer
