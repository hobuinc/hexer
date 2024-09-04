//ABELL
#include <set>

#include <hexer/BaseGrid.hpp>

#include "Path.hpp"
#include "exception.hpp"
#include <iostream>

namespace hexer
{

void BaseGrid::addPoint(Point p)
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
        HexId below = edgeHex(h, 0);
        HexId above = edgeHex(h, 3);
        if (!isDense(below))
            addRoot(h);
        removeRoot(above);
    }
}

void BaseGrid::handleSamplePoint(Point p)
{
    std::cout << "finding sample points: \n";
    m_sample.push_back(p);
    if (m_sample.size() >= m_maxSample) {
        double height = computeHexSize();
        processHeight(height);
        for (auto it = m_sample.begin(); it != m_sample.end(); ++it) {
            addPoint(*it);
        }
        m_sample.clear();
    }
}

// debug function
void BaseGrid::findPossibleRoots()
{
    std::cout << "finding possible roots: \n";
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
    // does this work if m_counts[h] doesn't exist?
    return m_counts[h] > m_denseLimit;
}

void BaseGrid::findShapes()
{
    std::cout << "running findShapes(): \n";
    if (m_possibleRoots.empty())
        throw hexer_error("No areas of sufficient density - no shapes. "
            "Decrease density or area size.");

    int shapeNum = 0;
    std::cout << m_possibleRoots.size() << ", " << shapeNum << "\n";
    while (m_possibleRoots.size())
    {
        HexId root = *m_possibleRoots.begin();
        findShape(root, shapeNum++);
        Path& p = m_paths.back();
        std::cerr << "Added path = " << (void *)&p << "!\n";
    }
    for (Path& p : m_paths)
        std::cerr << "Path = " << (void *)&p << "!\n";

    std::set<void *> hexPaths;
    for (auto& [hexId, pathPtr] : m_hexPaths)
        hexPaths.insert((void *)pathPtr);

    std::cerr << "\n";
    for (void *v : hexPaths)
        std::cerr << "Hex path = " << v << "!\n";
}

void BaseGrid::findShape(HexId root, int pathNum)
{
    std::cout << "findShape() " << pathNum << "\n";
    m_paths.push_back(Path(pathNum));
    Path& path = m_paths.back();

    Segment first(root, 0);
    Segment cur(root, 0);
    do {
        if (cur.horizontal())
        {
            m_possibleRoots.erase(cur.hex);
            HexId pathHex = (cur.edge == 0 ? cur.hex : edgeHex(cur.hex, 3));
            m_hexPaths.insert({pathHex, &path});
        }
        path.add(cur);
        path.addPoint(findPoint(cur));
        const auto& [left, right] = nextSegments(cur);
        cur = isDense(left.hex) ? left : right;
    } while (cur != first);
    path.addPoint(findPoint(first));
}

std::pair<Segment, Segment> BaseGrid::nextSegments(const Segment& s) const
{
    static const int next[] { 1, 2, 3, 4, 5, 0 };
    static const int prev[] { 5, 0, 1, 2, 3, 4 };

    Segment right(s.hex, next[s.edge]);
    Segment left(edgeHex(right.hex, right.edge), prev[s.edge]);
    return { left, right };
}

void BaseGrid::findParentPaths()
{
    std::vector<Path> roots;
    std::cout << m_paths.size() <<" paths\n";
    for (size_t i = 0; i < m_paths.size(); ++i) {
        Path p = m_paths[i];
        // the only difference between parentOrChild in the two grid types
        // is whether they look down i or j, I think.
        parentOrChild(p);

        !p.parent() ?  roots.push_back(p) : p.parent()->addChild(p);
    }
    std::cout << roots.size() <<" roots\n";
    for (size_t i = 0; i < roots.size(); ++i) {
        roots[i].finalize(CLOCKWISE);
    }
    m_paths = roots;
}

double BaseGrid::distance(const Point& p1, const Point& p2)
{
    double xdist = p2.m_x - p1.m_x;
    double ydist = p2.m_y - p1.m_y;
    return std::sqrt(xdist * xdist + ydist * ydist);
}

// Compute hex size based on distance between consecutive points and
// density.  The probably needs some work based on more data.
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

} // namespace hexer
