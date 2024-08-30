#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Mathpair.hpp"
#include "Path.hpp"
#include "HexId.hpp"

#include <h3/include/h3api.h>

namespace hexer
{

class HexGrid;
class H3Grid;

class BaseGrid
{
public:
    void addPoint(Point p);
    void findShapes();
    void findParentPaths();
    void setSampleSize(int num)
        {m_maxSample = num; }
    std::vector<Path> const& rootPaths() const
        { return m_paths; }
    void setGrid(std::pair<HexId, int> cell)
        { m_counts.insert(cell); }
    void findPossibleRoots();

protected:
    BaseGrid(int dense_limit) : m_denseLimit{dense_limit}
    {}
    virtual ~BaseGrid() = default;
    int increment(HexId hex);
    int m_maxSample;
    std::unordered_map<HexId, Path *> m_hexPaths;
    std::unordered_map<HexId, int> m_counts;

private:
    //ABELL - Fix these.
    virtual bool sampling() const = 0;
    virtual HexId findHexagon(Point p) = 0;
    virtual HexId edgeHex(HexId hex, int edge) const = 0;
    virtual void processHeight(double height) = 0;
    virtual void parentOrChild(Path p) = 0;
    virtual Point findPoint(Segment s) = 0; 

    void handleSamplePoint(Point p);
    void addRoot(HexId hex);
    void removeRoot(HexId hex);
    bool isDense(HexId hex);
    void findShape(HexId root, int pathNum);
    double distance(const Point& p1, const Point& p2);
    double computeHexSize();
    std::pair<Segment, Segment> nextSegments(const Segment& s) const;

    std::vector<Point> m_sample;
    std::unordered_set<HexId> m_possibleRoots;
    std::vector<Path> m_paths;
    int m_denseLimit;
};

} // namespace hexer