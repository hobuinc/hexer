#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Hexagon.hpp"
#include "Mathpair.hpp"
#include "Path.hpp"
#include "Processor.hpp"
#include "export.hpp"

#include <h3/include/h3api.h>

namespace hexer
{

class BaseGrid
{
public:
    BaseGrid(int dense_limit = 10) : m_denseLimit{dense_limit}
    {}

    void addPoint(Point p);
    void findShapes();
    void setSampleSize(int num)
        {m_maxSample = num; }

protected:
    int increment(CoordIJ hex);
    int m_maxSample;

private:
    //ABELL - Fix these.
    virtual bool sampling() const
        { return true; }
    virtual CoordIJ findHexagon(Point p) = 0;
    virtual CoordIJ edgeHex(CoordIJ hex, int edge) = 0;
    virtual void processSample(double height)
    {}
    void handleSamplePoint(Point p)
    {}

    void addRoot(CoordIJ hex);
    void removeRoot(CoordIJ hex);
    bool isDense(CoordIJ hex);
    void findShape(CoordIJ root, int pathNum);

    std::vector<Point> m_sample;
    std::unordered_map<CoordIJ, int> m_counts;
    std::unordered_set<CoordIJ> m_possibleRoots;
    std::unordered_map<CoordIJ, int> m_hexPaths;
    int m_denseLimit;
};

} // namespace hexer