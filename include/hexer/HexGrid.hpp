/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Andrew Bell andrew.bell.ia at gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#pragma once

#include <unordered_map>
#include <set>

#include "Draw.hpp"
#include "Hexagon.hpp"
#include "Mathpair.hpp"
#include "Path.hpp"
#include "Segment.hpp"
#include "export.hpp"

namespace hexer
{

class HexIter;

static const double SQRT_3 = 1.732050808; 

class HEXER_DLL HexGrid
{
    friend class HexIter;
public:
    HexGrid(int dense_limit);
    HexGrid(double height, int dense_limit) : m_dense_limit(dense_limit)
        { initialize(height); }

    ~HexGrid()
    {
        for (std::vector<Path*>::size_type i = 0; i < m_paths.size(); i++)
            delete m_paths[i];
    }

    bool dense(Hexagon *h);
    void addPoint(double x, double y)
        { addPoint(Point(x, y)); }
    void addPoint(Point p);
    void processSample();
    void findShapes();
    void findParentPaths();
    void extractShapes();
    void dumpInfo();
    void drawHexagons();
    Hexagon *getHexagon(int x, int y);
    Hexagon *getHexagon(const Coord& c)
        { return getHexagon(c.m_x, c.m_y); }
    void addDenseHexagon(int x, int y);
    HexIter hexBegin();
    HexIter hexEnd();
    double width() const
        { return m_width; }
    double height() const
        { return m_height; }
    Point const& offset(int idx) const
        { return m_offsets[idx]; }
    Point centerOffset(int idx) const
        { return (m_offsets[idx] - m_center_offset); }
    Point const& origin() const 
        { return m_origin; }
    int denseLimit() const
        { return m_dense_limit; }
    std::vector<Path *> const& rootPaths() const
        { return m_paths; }
    void setSampleSize(unsigned sampleSize)
        { m_maxSample = sampleSize; }
    void toWKT(std::ostream&) const;
    size_t densePointCount() const;
    
private:
    void initialize(double height);
    Hexagon *findHexagon(Point p);
    void findShape(Hexagon *hex);
    void findHole(Hexagon *hex);
    void cleanPossibleRoot(Segment s, Path *p);
    void findParentPath(Path *p);
    void markNeighborBelow(Hexagon *hex);

    /// Height of the hexagons in the grid (2x apothem)
    double m_height;
    /// Width of the hexagons in the grid
    double m_width;
    /// Origin of the hex grid in point coordinates.
    Point m_origin;
    /// Offsets of vertices of hexagon, going anti-clockwise from upper-left
    Point m_offsets[6];
    /// Offset of the center of the hexagons.
    Point m_center_offset;
    /// Map of hexagons based on keys.
    typedef std::unordered_map<uint64_t, Hexagon> HexMap;
    HexMap m_hexes;
    /// Set of dense hexagons with non-dense neighbors above.
    typedef std::set<Hexagon *, HexCompare> HexSet;
    HexSet m_pos_roots;
    /// Map of roots and their associated paths.
    typedef std::unordered_map<Hexagon *, Path *> HexPathMap;
    HexPathMap m_hex_paths;
    /// List of paths
    std::vector<Path *> m_paths;
    /// Number of points that must lie in a hexagon for it to be interesting.
    int m_dense_limit;
    /// Minimum y - 1.
    int m_miny;
    /// Vector of points to use to determine hex height.
    std::vector<Point> m_sample;
    /// Maximum sample size.
    unsigned m_maxSample;
};

} // namespace

