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

#include "gdal.h"

#include <hexer/Mathpair.hpp>

namespace hexer
{

/* void process(HexGrid *grid, PointReader reader)
{
    double x, y;
    void* context;

    while (reader(x, y, context))
        grid->addPoint(x, y);
    grid->findShapes();
    grid->findParentPaths();
} */

void processLaz(HexGrid *grid, std::ifstream& file)
{
    lazperf::reader::generic_file l(file);

    size_t count = l.pointCount();

    lazperf::header14 h = l.header();
    uint16_t len = h.point_record_length;
    std::vector<char> buf(len, 0);
    char* buf_data = buf.data();
    std::cout << "adding points: ";
    if(count < 10000)
        grid->setSampleSize(count);
    else 
        grid->setSampleSize(10000);

    for(size_t i = 0; i < count; i ++) {
        l.readPoint(buf_data);

        int32_t *pos = (int32_t *)buf_data;
        int32_t x_int = *pos;
        pos++;
        int32_t y_int = *pos;

        double x = x_int * h.scale.x + h.offset.x;
        double y = y_int * h.scale.y + h.offset.y;
        Point p{x, y};
        grid->addPoint(p);
    }
    std::cout << "points added. finding shapes: ";
    grid->findShapes();
    std::cout << "shapes found. finding paths: ";
    grid->findParentPaths();
    std::cout << "paths found. ";
}

void processH3(H3Grid *grid, std::ifstream& file) 
{
    lazperf::reader::generic_file l(file);

    size_t count = l.pointCount();

    lazperf::header14 h = l.header();
    uint16_t len = h.point_record_length;
    std::vector<char> buf(len, 0);
    char* buf_data = buf.data();
    std::cout << "adding points: ";
    if(count < 10000)
        grid->setSampleSize(count);
    else 
        grid->setSampleSize(10000);

    // add: verify WGS84 w/ gdal

    for(size_t i = 0; i < count; i ++) {
        l.readPoint(buf_data);

        int32_t *pos = (int32_t *)buf_data;
        int32_t x_int = *pos;
        pos++;
        int32_t y_int = *pos;

        double x_rad = degsToRads(x_int * h.scale.x + h.offset.x);
        double y_rad = degsToRads(y_int * h.scale.y + h.offset.y);
        Point p{x_rad, y_rad};
        
        grid->addPoint(p);
    }
    std::cout << "points added. finding shapes: ";
    grid->findShapes();
    std::cout << "shapes found. finding paths: ";
    grid->findParentPaths();
    std::cout << "paths found. ";
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

    os << " with GDAL " << GDALVersionInfo("RELEASE_NAME");

    return os.str();
}

} //namespace hexer
