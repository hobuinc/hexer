/* #include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <hexer/HexGrid.hpp>
#include <hexer/Processor.hpp>
#include <hexer/HexIter.hpp>
#include <hexer/Utils.hpp>
#include "test_main.hpp"
#include <lazperf/readers.hpp>
#include <lazperf/las.hpp>
#include <h3/include/h3api.h>

namespace hexer {

void inputTest(std::string const& input, double edge = 0.0, int density = 0) {

    std::unique_ptr<HexGrid> grid;

    if (density == 0)
        density = 10;
    if (edge == 0.0) 
        grid.reset(new HexGrid(density));
    else
        grid.reset(new HexGrid(edge, density));

    HexGrid *a = grid.get();
    std::ifstream file(input, std::ios::binary);
    processLaz(a, file);

    int maxX = -1000000;
    int minX = 1000000;
    int maxY = -1000000;
    int minY = 1000000;
    int cnt = 0;

    for (auto it = a->hexBegin(); it != a->hexEnd(); ++it) {
        HexInfo info = *it;
        maxX = std::max(info.m_pos.m_x, maxX);
        minX = std::min(info.m_pos.m_x, minX);
        maxY = std::max(info.m_pos.m_y, maxY);
        minY = std::min(info.m_pos.m_y, minY);
        cnt++;
    }
    EXPECT_EQ(maxX, 0);
    EXPECT_EQ(minX, -23);
    EXPECT_EQ(maxY, 1);
    EXPECT_EQ(minY, -9); 
}


TEST(gridtest, grid_output_las) {
    inputTest(testFile("autzen_trim.las"));
}

TEST(gridtest, grid_output_laz) {
    inputTest(testFile("autzen_trim.laz")); 
}

TEST(gridtest, test_h3_exist) {
    LatLng location;
    location.lat = degsToRads(40.689167);
    location.lng = degsToRads(-74.044444);
    int resolution = 10;
    H3Index index;
    EXPECT_EQ(latLngToCell(&location, resolution, &index), E_SUCCESS); 
}
} //namespace hexer */