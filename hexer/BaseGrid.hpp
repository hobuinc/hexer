#pragma once

#include <vector>
#include <list>
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
    void addPoint(Point& p);
    void findShapes();
    void findParentPaths();
    bool isDense(HexId hex);
    void toWKT(std::ostream& output) const;
    
    // test function: adds pre-defined hexagon coordinates to the grid 
    void setHexes(const std::vector<HexId>& hexes);

    void setSampleSize(int num)
        {m_maxSample = num; }
    std::vector<Path *> const& rootPaths() const
        { return m_roots; }
    // returns all hexagons in the grid and their counts
    std::unordered_map<HexId, int> const& getHexes()
        { return m_counts; }

    virtual void addXY(double& x, double& y) = 0;
    virtual Point findPoint(Segment& s) = 0;
    virtual bool isH3() = 0;

    virtual H3Index ij2h3(HexId ij)
        { return 0; }
    virtual HexId h32ij(H3Index h3)
        { return {0,0}; }

protected:
    BaseGrid(int dense_limit) : m_denseLimit{dense_limit}
    {}
    int increment(HexId hex);

    int m_maxSample;
    ///   map of cells bordering paths at side 0 or 3
    std::unordered_map<HexId, Path *> m_hexPaths;
    ///   map of all hexagons containing points, and the number of points within.
    std::unordered_map<HexId, int> m_counts;

private:
    virtual bool sampling() const = 0;
    virtual HexId findHexagon(Point p) = 0;
    virtual HexId edgeHex(HexId hex, int edge) const = 0;
    virtual void processHeight(double height) = 0;
    virtual bool inGrid(HexId& h) = 0;
    virtual HexId moveCoord(HexId& h) = 0;

    void handleSamplePoint(Point& p);
    void addRoot(HexId hex);
    void removeRoot(HexId hex);
    void findShape(HexId root);
    double distance(const Point& p1, const Point& p2);
    double computeHexSize();
    void parentOrChild(Path& p);
    std::pair<Segment, Segment> nextSegments(const Segment& s) const;

    ///   Vector of points to use to determine hex height
    std::vector<Point> m_sample;
    ///   Set of hexagons with non-dense neighbors at side 0
    std::unordered_set<HexId> m_possibleRoots;
    ///   List of all paths
    std::list<Path> m_paths;
    ///   List of pointers to paths in m_paths to be written as roots
    std::vector<Path *> m_roots;
    ///   Minimum number of points for a cell to be dense
    int m_denseLimit;
};

} // namespace hexer
