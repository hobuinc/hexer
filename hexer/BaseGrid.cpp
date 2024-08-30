#include <hexer/BaseGrid.hpp>

#include "Path.hpp"
#include "exception.hpp"

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
    m_sample.push_back(p);
    if (m_sample.size() >= m_maxSample) {
        double height = computeHexSize(m_sample, m_denseLimit);
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
    if (m_possibleRoots.empty())
        throw hexer_error("No areas of sufficient density - no shapes. "
            "Decrease density or area size.");

    int shapeNum = 0;
    while (m_possibleRoots.size())
    {
        HexId root = *m_possibleRoots.begin();
        findShape(root, shapeNum++);
    }
}

void BaseGrid::findShape(HexId root, int pathNum)
{
    Path path(pathNum);

    Segment first(root, 0);
    Segment cur(root, 0);
    do {
        if (cur.horizontal())
        {
            m_possibleRoots.erase(cur.hex);
            HexId pathHex = (cur.edge == 0 ? cur.hex : edgeHex(cur.hex, 3));
            m_hexPaths.emplace(pathHex, &path);
        }
        path.add(cur);
        path.addPoint(findPoint(cur));
        const auto& [left, right] = nextSegments(cur);
        cur = isDense(left.hex) ? left : right;
    } while (cur != first);
    m_paths.push_back(std::move(path));
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
    for (size_t i = 0; i < m_paths.size(); ++i) {
        Path p = m_paths[i];
        // the only difference between parentOrChild in the two grid types
        // is whether they look down i or j, I think.
        parentOrChild(p);

        !p.parent() ?  roots.push_back(p) : p.parent()->addChild(p);
    }
    for (size_t i = 0; i < roots.size(); ++i) {
        roots[i].finalize(CLOCKWISE);
    }
    m_paths = roots;
}

} // namespace hexer