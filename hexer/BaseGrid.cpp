#include <set>

#include <hexer/BaseGrid.hpp>

#include "Path.hpp"
#include "exception.hpp"
#include <iostream>

namespace hexer
{

void BaseGrid::addPoint(Point& p)
{
    if (sampling())
    {
        handleSamplePoint(p);
        return;
    }

    HexId h = findHexagon(p);
    int count = increment(h);
    if (count == m_denseLimit)
    {
        HexId above = edgeHex(h, 0);
        HexId below = edgeHex(h, 3);
        if (!isDense(above))
            addRoot(h);
        removeRoot(below);
    }
}

void BaseGrid::setHexes(const std::vector<HexId>& ids)
{
    for (HexId h : ids)
    {
        m_counts[h] = m_denseLimit + 1;
        HexId above = edgeHex(h, 0);
        HexId below = edgeHex(h, 3);
        if (!isDense(above))
            addRoot(h);
        removeRoot(below);
    }
}

void BaseGrid::handleSamplePoint(Point& p)
{
    m_sample.push_back(p);
    if (m_sample.size() >= m_maxSample) {
        double height = computeHexSize();
        processHeight(height);
        for (Point p : m_sample) {
            addPoint(p);
        }
        m_sample.clear();
    }
}

// debug function
void BaseGrid::findPossibleRoots()
{
    for (auto it = m_counts.begin(); it != m_counts.end(); ++it) {
        HexId hex = it->first;
        HexId below = edgeHex(hex, 0);
        if (!isDense(below))
            addRoot(hex);
    }
}

void BaseGrid::addRoot(HexId h)
{
    m_possibleRoots.insert(h);
}

void BaseGrid::removeRoot(HexId h)
{
    m_possibleRoots.erase(h);
}

int BaseGrid::increment(HexId h)
{
    int& i = m_counts[h];
    i++;
    return i;
}

bool BaseGrid::isDense(HexId h)
{
    return m_counts[h] >= m_denseLimit;
}

// Find the full boundary around our dense hexagons
void BaseGrid::findShapes()
{
    if (m_possibleRoots.empty())
        throw hexer_error("No areas of sufficient density - no shapes. "
            "Decrease density or area size.");

    while (m_possibleRoots.size())
    {
        HexId root = *m_possibleRoots.begin();
        findShape(root);
    }
}

// Walk one contiguous segment of the boundary
void BaseGrid::findShape(HexId root)
{
    m_paths.push_back(Path(root));
    Path& path = m_paths.back();

    const Segment first(root, 0);
    Segment cur(root, 0);

    do {
        if (cur.horizontal())
        {
            if (cur.edge == 0)
                m_possibleRoots.erase(cur.hex);
            HexId pathHex = (cur.edge == 0 ? cur.hex : edgeHex(cur.hex, 3));
            m_hexPaths.insert({pathHex, &path});
        }
        path.addPoint(findPoint(cur));

        const auto& [left, right] = nextSegments(cur);
        cur = isDense(left.hex) ? left : right;
    } while (cur != first);

    path.addPoint(findPoint(cur));
}

// Finds the possibilities for the next boundary segment, moving clockwise
std::pair<Segment, Segment> BaseGrid::nextSegments(const Segment& s) const
{
    //    ____
    //   /    \  <---- Current segment: edge 4 of (0,0)
    //  / 0,0  \__v----------- Possible next segment, left: edge 3 of (1,-1)
    //  \      /<---\----- Possible next segment, right: edge 5 of (0,0)
    //   \____/ 1,-1 \
    //   /    \      /
    //  / 0,-1 \____/
    //  \      /    \
    //   \____/      \
    //
    static const int next[] { 1, 2, 3, 4, 5, 0 };
    static const int prev[] { 5, 0, 1, 2, 3, 4 };

    Segment right(s.hex, next[s.edge]);
    Segment left(edgeHex(s.hex, right.edge), prev[s.edge]);
    return { left, right };
}

// Determine whether a path is enclosed within another
void BaseGrid::findParentPaths()
{
    for (auto& p : m_paths) {
        // the only real difference between parentOrChild in the two grid 
        // types is whether they look down i or j.
        parentOrChild(p);

        !p.parent() ?  m_roots.push_back(&p) : p.parent()->addChild(&p);
    }
    for (size_t i = 0; i < m_roots.size(); ++i) {
        m_roots[i]->finalize(CLOCKWISE);
    }
}

double BaseGrid::distance(const Point& p1, const Point& p2)
{
    double xdist = p2.m_x - p1.m_x;
    double ydist = p2.m_y - p1.m_y;
    return std::sqrt(xdist * xdist + ydist * ydist);
}

// Compute hex size based on distance between consecutive points and
// density
double BaseGrid::computeHexSize()
{
    double dist = 0;
    for (std::vector<Point>::size_type i = 0; i < m_sample.size() - 1; ++i)
    {
        Point p1 = m_sample[i];
        Point p2 = m_sample[i + 1];
        dist += distance(p1, p2);
    }
    return ((m_denseLimit * dist) / m_sample.size());
}

void BaseGrid::toWKT(std::ostream& output) const
{
    auto outputPath = [this,&output](Path *p)
    {
        output << "(";
        p->toWKT(output);
        output << ")";
    };

    std::vector<Path *> paths = rootPaths();

    output << "MULTIPOLYGON (";

    auto it = paths.begin();
    if (it != paths.end())
        outputPath(*it++);
    for (; it != paths.end(); ++it)
    {
        output << ",";
        outputPath(*it);
    }
    output << ")";
}

} // namespace hexer
