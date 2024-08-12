#pragma once

#include <vector>

#include "export.hpp"
#include "Mathpair.hpp"
//#include "Path.hpp"
#include "exception.hpp"
//#include "H3Grid.hpp"

#include <h3/include/h3api.h>

namespace hexer
{

class H3Grid;

using DirEdge = H3Index;

enum H3Orientation
{
    H3CLOCKWISE,     // Hole
    H3ANTICLOCKWISE  // Outer
};

class HEXER_DLL H3Path
{
public:
    H3Path(H3Grid *m_grid, H3Orientation dir, const CoordIJ hex) :
        m_grid(m_grid), m_parent(NULL), m_direction(dir),
        m_root_hex(hex)
    {}

    ~H3Path()
    {
        for (std::vector<H3Path*>::size_type i = 0; i < m_children.size(); ++i)
            delete m_children[i];
    }

    H3Path *parent()
        { return m_parent; }
    void push_back(const DirEdge& s)
        { m_segs.push_back(s); }
    void setParent(H3Path *p)
        { m_parent = p; }
    size_t pathLength() const
        { return m_segs.size(); }
    DirEdge getPath(int i) const
        { return m_segs[i];}
    void addChild(H3Path *p)
        { m_children.push_back(p); }
    void finalize(H3Orientation o)
    {
        m_direction = o;
        for (size_t i = 0; i < m_children.size(); ++i)
            m_children[i]->finalize(o == H3CLOCKWISE ? H3ANTICLOCKWISE : H3CLOCKWISE);
    }
    CoordIJ rootHex() const
        { return m_root_hex; }
    std::vector<H3Path *> subPaths() const
        { return m_children; }
    std::vector<Point> getPoints() const;
private:
    /// Grid that owns the path.
    H3Grid *m_grid;
    /// Parent path (NULL if root path)
    H3Path *m_parent;
    /// Children
    std::vector<H3Path *> m_children;
    /// Orientation: counter-clockwise (outer path), or clockwise (inner path)
    H3Orientation m_direction;
    /// segments
    std::vector<DirEdge> m_segs;
    /// example hexagon along path
    const CoordIJ m_root_hex;

};

}// namespace hexer