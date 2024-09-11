#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "test_main.hpp"

#include <hexer/HexGrid.hpp>
#include <hexer/Processor.hpp>
#include <hexer/Utils.hpp>
#include <lazperf/readers.hpp>
#include <lazperf/las.hpp>
#include <h3/include/h3api.h>

namespace hexer {

void inputTest(std::string const& input) {

    std::unique_ptr<HexGrid> grid;

    int density = 10;
    grid.reset(new HexGrid(density));

    std::ifstream file(input, std::ios::binary);
    processLaz(*grid, file);

    int maxX = -1000000;
    int minX = 1000000;
    int maxY = -1000000;
    int minY = 1000000;
    int cnt = 0;
    const std::unordered_map<HexId, int>& hexes = grid->getMap();
    for (auto it = hexes.begin(); it != hexes.end(); it++) {
        if (grid->isDense(it->first)){
            maxX = std::max(it->first.i, maxX);
            minX = std::min(it->first.i, minX);
            maxY = std::max(it->first.j, maxY);
            minY = std::min(it->first.j, minY);
        }
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
} //namespace hexer