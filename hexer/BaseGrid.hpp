#pragma once

#include <set>

#include "Hexagon.hpp"
#include "Mathpair.hpp"
#include "Path.hpp"
#include "Segment.hpp"
#include "export.hpp"

namespace hexer
{

class HEXER_DLL BaseGrid
{
public:
    std::vector<Point> getSamples() const
        { return m_sample; }
    void setSampleSize(unsigned sampleSize)
        { m_max_sample = sampleSize; }
    void addPoint(Point p);
    void findShapes();

protected:
    std::vector<Point> m_sample;
    /// Map of hexagons based on keys.
    typedef std::unordered_map<uint64_t, Hexagon> HexMap;
    HexMap m_hexes;
private:
    bool isDense(Hexagon *h);
    void findParentPaths();
    void findShape(Hexagon *h);
    void addSegment(Segment s, Path *p);
    void markNeighborBelow(Hexagon *h);

    int m_dense_limit;
    int m_max_sample;
    /// Set of dense hexagons with non-dense neighbors above.
    // need to make sure HexCompare operates on subclass (h->less() virtual function)
    typedef std::set<Hexagon *, HexCompare> HexSet;
    HexSet m_pos_roots;
    /// Map of roots and their associated paths.
    typedef std::unordered_map<Hexagon *, Path *> HexPathMap;
    HexPathMap m_hex_paths;
    std::vector<Path *> m_paths;
};

} // namespace hexer