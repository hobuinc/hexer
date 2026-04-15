#include "test_main.hpp"
#include <hexer/H3grid.hpp>
#include <hexer/HexGrid.hpp>
#include <hexer/HexId.hpp>

namespace hexer 
{

Path *findChild(const std::vector<Path *>& paths, int numPoints, int numChildren)
{
    for (Path *path : paths)
    {
        if (path->numPoints() == numPoints && path->numChildren() == numChildren)
            return path;
    }
    return nullptr;
}

const std::vector<Path*> insertGrid(H3Grid *grid) 
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
    auto paths = grid->rootPaths();
    return paths;
}

TEST(pathstest, test_paths_h3) 
{
    std::unique_ptr<H3Grid> grid;
    grid.reset(new H3Grid(10, 1));

    std::vector<Path*> paths = insertGrid(grid.get());

    ASSERT_EQ(paths.size(), 1u);
    EXPECT_EQ(paths[0]->numPoints(), 55);
    EXPECT_EQ(paths[0]->numChildren(), 2);
    
    std::vector<Path *> child_0 = paths[0]->subPaths();
    ASSERT_EQ(child_0.size(), 2u);
    Path *branch_39 = findChild(child_0, 39, 2);
    Path *branch_15 = findChild(child_0, 15, 0);
    ASSERT_NE(branch_39, nullptr);
    ASSERT_NE(branch_15, nullptr);
    
    std::vector<Path *> child_0_0 = branch_39->subPaths();
    ASSERT_EQ(child_0_0.size(), 2u);
    Path *branch_19 = findChild(child_0_0, 19, 1);
    Path *branch_7 = findChild(child_0_0, 7, 0);
    ASSERT_NE(branch_19, nullptr);
    ASSERT_NE(branch_7, nullptr);
    
    std::vector<Path *> child_0_0_0 = branch_19->subPaths();
    ASSERT_EQ(child_0_0_0.size(), 1u);
    EXPECT_EQ(child_0_0_0[0]->numPoints(), 7);
    EXPECT_EQ(child_0_0_0[0]->numChildren(), 0);
}

const std::vector<Path*> insertGrid(HexGrid *grid) 
{
    grid->setHexes(
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
    grid->findShapes();
    grid->findParentPaths();
    auto paths = grid->rootPaths();
    return paths;
}

TEST(pathstest, test_paths_hexer) 
{
    std::unique_ptr<HexGrid> grid;
    grid.reset(new HexGrid(1, 10));

    std::vector<Path*> paths = insertGrid(grid.get());

    ASSERT_EQ(paths.size(), 1u);
    EXPECT_EQ(paths[0]->numPoints(), 55);
    EXPECT_EQ(paths[0]->numChildren(), 2);

    std::vector<Path *> child_0 = paths[0]->subPaths();
    ASSERT_EQ(child_0.size(), 2u);
    Path *branch_39 = findChild(child_0, 39, 2);
    Path *branch_15 = findChild(child_0, 15, 0);
    ASSERT_NE(branch_39, nullptr);
    ASSERT_NE(branch_15, nullptr);

    std::vector<Path *> child_0_0 = branch_39->subPaths();
    ASSERT_EQ(child_0_0.size(), 2u);
    Path *branch_7 = findChild(child_0_0, 7, 0);
    Path *branch_19 = findChild(child_0_0, 19, 1);
    ASSERT_NE(branch_7, nullptr);
    ASSERT_NE(branch_19, nullptr);

    std::vector<Path *> child_0_0_1 = branch_19->subPaths();
    ASSERT_EQ(child_0_0_1.size(), 1u);
    EXPECT_EQ(child_0_0_1[0]->numPoints(), 7);
    EXPECT_EQ(child_0_0_1[0]->numChildren(), 0);
}
} // namespace hexer 
