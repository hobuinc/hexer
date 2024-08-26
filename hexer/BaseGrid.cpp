#include <hexer/BaseGrid.hpp>

namespace hexer
{
bool BaseGrid::isDense(Hexagon * h)
{
    return h->count() >= m_dense_limit;
}
void BaseGrid::addPoint(Point p)
{
    if (sampling())
    {
        m_sample.push_back(p);
        if (m_sample.size() >= m_maxSample)
            processSample();
        return;
    }

    Hexagon *h = findHexagon(p);
    h->increment();
    if (!h->dense())
    {
        if (isDense(h))
        {
            h->setDense();
            // hexgrid finds m_miny here: find somewhere else to do that?
            if (h->possibleRoot())
                m_pos_roots.insert(h);
            markNeighborBelow(h);
        }
    }
}

void BaseGrid::findShapes()
{
    if (m_pos_roots.empty())
    {
        throw hexer_error("No areas of sufficient density - no shapes. "
            "Decrease density or area size.");
    }

    while (m_pos_roots.size())
    {
        Hexagon *h = *m_pos_roots.begin();
        findShape(h);
    }
}

void BaseGrid::findShape(Hexagon *hex)
{
    if (!hex)
        throw hexer_error("hexagon was null!");

    Path *p = createPath(); //new Path(this, CLOCKWISE);
    Segment first(hex, 0);
    Segment cur(first);
    do {
        cleanPossibleRoot(cur, p);
        p->push_back(cur);
        Segment next = cur.leftClockwise(this);
        if (!next.hex()->dense())
            next = cur.rightClockwise(this);
        cur = next;
    } while (cur != first);
    m_paths.push_back(p);
}
} // namespace hexer