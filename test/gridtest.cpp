#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <hexer/HexGrid.hpp>
#include <hexer/HexIter.hpp>
#include <hexer/Processor.hpp>
#include <hexer/Utils.hpp>
#include "test_main.hpp"
#include <lazperf/readers.hpp>
#include <lazperf/las.hpp>

namespace hexer {

void processTest(HexGrid *grid, std::ifstream& file) {
    
    lazperf::reader::generic_file l(file);

    size_t count = l.pointCount();

    lazperf::header14 h = l.header();
    uint16_t len = h.point_record_length;
    std::vector<char> buf(len, 0);
    char* buf_data = buf.data();

    for(size_t i = 0; i < count; i ++) {
        l.readPoint(buf_data);

        int32_t *pos = (int32_t *)buf_data;
        int32_t x_int = *pos;
        pos++;
        int32_t y_int = *pos;

        double x = x_int * h.scale.x + h.offset.x;
        double y = y_int * h.scale.y + h.offset.y;

        grid->addPoint(x, y);
    }
}

void inputTest(std::string const& input, double edge = 0.0, int density = 0) {

    std::unique_ptr<HexGrid> grid;

    if (density == 0)
        density = 10;
    if (edge == 0.0) 
        grid.reset(new HexGrid(density));
    else
        grid.reset(new HexGrid(edge, density));

    std::ifstream file(input, std::ios::binary);
    processTest(grid.get(), file);
}
} //namespace hexer

TEST(gridtest, grid_output) {
    int maxX = -1000000;
    int minX = 1000000;
    int maxY = -1000000;
    int minY = 1000000;
    int cnt = 0;

    inputTest(testFile("autzen_trim.las"));

    for (auto it = grid->hexBegin(); it != grid->hexEnd(); ++it)
    {
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