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
    void setHexes(const std::vector<HexId>& hexes);
    void addPoint(Point& p);
    void findShapes();
    void findParentPaths();
    bool isDense(HexId hex);
    void toWKT(std::ostream& output) const;

    void setSampleSize(int num)
        {m_maxSample = num; }
    std::vector<Path *> const& rootPaths() const
        { return m_roots; }
    void setGrid(std::pair<HexId, int> cell)
        { m_counts.insert(cell); }
    void findPossibleRoots();
    const std::unordered_map<HexId, int> &getMap()
        { return m_counts; }

    virtual Point findPoint(Segment& s) = 0;

protected:
    BaseGrid(int dense_limit) : m_denseLimit{dense_limit}
    {}
    int increment(HexId hex);

    int m_maxSample;
    /// @brief map of cells bordering paths at side 0 or 3
    std::unordered_map<HexId, Path *> m_hexPaths;
    /// @brief map of all hexagons containing points, and the number of points within.
    std::unordered_map<HexId, int> m_counts;

private:
    virtual bool sampling() const = 0;
    virtual HexId findHexagon(Point p) = 0;
    virtual HexId edgeHex(HexId hex, int edge) const = 0;
    virtual void processHeight(double height) = 0;
    virtual bool inGrid(int i) = 0;
    virtual void parentOrChild(Path& p) = 0;

    //void parentOrChild(Path p);
    void handleSamplePoint(Point& p);
    void addRoot(HexId hex);
    void removeRoot(HexId hex);
    void findShape(HexId root);
    double distance(const Point& p1, const Point& p2);
    double computeHexSize();
    std::pair<Segment, Segment> nextSegments(const Segment& s) const;

    /// @brief Vector of points to use to determine hex height
    std::vector<Point> m_sample;
    /// @brief Set of hexagons with non-dense neighbors at side 0
    std::unordered_set<HexId> m_possibleRoots;
    /// @brief List of all paths
    std::list<Path> m_paths;
    /// @brief List of pointers to paths in m_paths to be written as roots
    std::vector<Path *> m_roots;
    /// @brief Minimum number of points for a cell to be dense
    int m_denseLimit;
};

} // namespace hexer
