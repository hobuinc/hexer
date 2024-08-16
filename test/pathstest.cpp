#include "test_main.hpp"
#include <hexer/HexGrid.hpp>
#include <h3/include/h3api.h>
/*
namespace hexer 
{
std::vector<H3Path *> insertGrid(H3Grid *grid) {
    for (CoordIJ ij : std::vector<CoordIJ>(
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
            }))
        grid->setGrid(std::make_pair(ij, 1));
    LatLng location;
    location.lat = degsToRads(40.689167);
    location.lng = degsToRads(-74.044444);
    int resolution = grid->getRes();
    H3Index index;
    EXPECT_EQ(latLngToCell(&location, resolution, &index), E_SUCCESS); 
    grid->setOrigin(index);
    grid->processPaths();
    const std::vector<hexer::H3Path *> paths = grid->rootPaths();
    return paths;
}

TEST(pathstest, test_paths) {
    std::unique_ptr<H3Grid> grid;
    grid.reset(new H3Grid(1, 10));
    std::vector<H3Path *> paths = insertGrid(grid.get());
    int counter(0);
    for (auto pi = paths.begin(); pi != paths.end(); ++pi) {
        std::cout << "root path " << counter << " = "
    } 
}
} // namespace hexer */