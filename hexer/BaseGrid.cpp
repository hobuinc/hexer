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
        if (m_sample.size() >= m_max_sample)
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
    // make sure correct segment type is created
    Segment first(hex, 0);
    Segment cur(first);
    do {
        addSegment(cur, p);
        p->push_back(cur);
        Segment next = cur.leftClockwise(this);
        if (!next.hex()->dense())
            next = cur.rightClockwise(this);
        cur = next;
    } while (cur != first);
    m_paths.push_back(p);
}

void BaseGrid::addSegment(Segment s, Path *p)
{
    if (s.possibleRoot(this))
        m_pos_roots.erase(s.hex());
    if (s.horizontal())
    {
        s.normalize(this);
        HexPathMap::value_type hexpath(s.hex(), p);
        m_hex_paths.insert(hexpath);
    }
}

void BaseGrid::findParentPaths()
{
    std::vector<Path *> roots;
    for (size_t i = 0; i < m_paths.size(); ++i)
    {
        Path *p = m_paths[i];
        findParentPath(p);
        // Either add the path to the root list or the parent's list of
        // children.
        !p->parent() ?  roots.push_back(p) : p->parent()->addChild(p);
    }
    for (size_t i = 0; i < roots.size(); ++i)
       roots[i]->finalize(CLOCKWISE);

    // In the end, the list of paths is just the root paths.  Children can
    // be retrieved from their parents.
    m_paths = roots;
}

} // namespace hexer