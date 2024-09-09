#include "test_main.hpp"
#include <hexer/H3grid.hpp>
#include <hexer/HexId.hpp>

namespace hexer 
{
std::vector<Path*> insertGrid(H3Grid *grid) 
{
    grid->setHexes(
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
    int resolution = grid->getRes();
    H3Index index;
    EXPECT_EQ(latLngToCell(&location, resolution, &index), E_SUCCESS); 
    grid->setOrigin(index);
    //grid->findPossibleRoots();
    grid->findShapes();
    grid->findParentPaths();
    const std::vector<Path*> paths = grid->rootPaths();
    return paths;
}

TEST(pathstest, test_paths) 
{
    std::unique_ptr<H3Grid> grid;
    grid.reset(new H3Grid(10, 1));
    std::vector<Path*> paths = insertGrid(grid.get());
    
    int counter(0);
    for (auto p : paths)
    {
        std::cout << "path " << counter <<" num points: " << p->numPoints();
        std::cout << " -- numChildren: " << p->numChildren() << "\n"; 
    }

    EXPECT_EQ(paths[0]->numPoints(), 55);
    EXPECT_EQ(paths[0]->numChildren(), 2);
    
    std::vector<Path *> child_0 = paths[0]->subPaths();
    EXPECT_EQ(child_0[0]->numPoints(), 39);
    EXPECT_EQ(child_0[1]->numPoints(), 15);
    EXPECT_EQ(child_0[0]->numChildren(), 2);
    EXPECT_EQ(child_0[1]->numChildren(), 0);
    
    std::vector<Path *> child_0_0 = child_0[0]->subPaths();
    EXPECT_EQ(child_0_0[0]->numPoints(), 19);
    EXPECT_EQ(child_0_0[1]->numPoints(), 7);
    EXPECT_EQ(child_0_0[0]->numChildren(), 1);
    EXPECT_EQ(child_0_0[1]->numChildren(), 0);
    
    std::vector<Path *> child_0_0_0 = child_0_0[0]->subPaths();
    EXPECT_EQ(child_0_0_0[0]->numPoints(), 7);
    EXPECT_EQ(child_0_0_0[0]->numChildren(), 0);
}
} // namespace hexer 