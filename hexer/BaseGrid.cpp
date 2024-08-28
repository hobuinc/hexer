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
        processSample(height);
        for (auto it = m_sample.begin(); it != m_sample.end(); ++it) {
            addPoint(*it);
        }
        m_sample.clear();
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
            m_hexPaths.emplace(pathHex, pathNum);
        }
        path.add(cur);
        Segment next = leftClockwiseSegment(cur);
        if (!hexDense(next.hex))
            next = rightClockwiseSegment(cur);
        cur = next;
    } while (cur != first);
    m_paths.push_back(std::move(path));
}

} // namespace hexer