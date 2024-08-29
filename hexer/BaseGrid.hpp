#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Mathpair.hpp"
#include "Path.hpp"
#include "Processor.hpp"
#include "export.hpp"
#include "HexId.hpp"

#include <h3/include/h3api.h>

namespace hexer
{

class BaseGrid
{
public:
    void addPoint(Point p);
    void findShapes();
    void findParentPaths();
    void setSampleSize(int num)
        {m_maxSample = num; }

protected:
    BaseGrid(int dense_limit) : m_denseLimit{dense_limit}
    {}
    int increment(HexId hex);
    int m_maxSample;
    std::unordered_map<HexId, Path *> m_hexPaths;
    std::unordered_map<HexId, int> m_counts;

private:
    //ABELL - Fix these.
    virtual bool sampling() const
        { return true; }
    virtual HexId findHexagon(Point p) = 0;
    virtual HexId edgeHex(HexId hex, int edge) = 0;
    virtual void processSample(double height)
    {}
    virtual void parentOrChild(Path p)
    {}

    void handleSamplePoint(Point p);
    void addRoot(HexId hex);
    void removeRoot(HexId hex);
    bool isDense(HexId hex);
    void findShape(HexId root, int pathNum);
    std::pair<Segment, Segment> nextSegments(const Segment& s) const;

    std::vector<Point> m_sample;
    std::unordered_set<HexId> m_possibleRoots;
    std::vector<Path> m_paths;
    int m_denseLimit;
};

} // namespace hexer