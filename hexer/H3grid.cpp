#include <assert.h>
#include <map>
#include <iostream>
#include <sstream>

#include <hexer/H3grid.hpp>
#include <hexer/exception.hpp>
#include <hexer/Processor.hpp>

#include <h3/include/h3api.h>

namespace hexer 
{

/*void H3Grid::addLatLng(LatLng *ll)
{
    if(m_res == -1) {
        m_sample.push_back(*ll);
        if (m_sample.size() >= m_maxSample)
            processH3Sample();
        return;
    }


    H3Index index(0);
    if (latLngToCell(ll, m_res, &index) != E_SUCCESS)
        throw hexer_error("H3 index not found!"); 
    if (m_map.size() == 0)
        m_origin = index;
    ++m_map[h32ij(index)];
}

void H3Grid::processGrid()
{
    // remove hexagons with fewer points than the density threshold
    for (auto it = m_map.begin(); it != m_map.end();) {
        if (it->second < m_dense_limit)
            it = m_map.erase(it);
        else {
            ++m_numdense;
            ++it;
        }
    }
    if (m_map.empty()) {
        throw hexer_error("No areas of sufficient density - no shapes. "
            "Decrease density or H3 resolution.");
    }
} */

Hexagon *H3Grid::findHexagon(Point p)
{
    H3Index index(0);
    LatLng ll{p.m_y, p.m_x};
    if (latLngToCell(&ll, m_res, &index) != E_SUCCESS)
        throw hexer_error("H3 index not found!");
    return getHexagon(h32ij(index));
}

Hexagon *H3Grid::getHexagon(CoordIJ ij)
{
    // Stick a hexagon into the map if necessary.
    HexMap::value_type hexpair(Hexagon::key(ij.i, ij.j), H3Hex(ij.i, ij.j));
    std::pair<HexMap::iterator,bool> retval;
    retval = m_hexes.insert(hexpair);
    HexMap::iterator it = retval.first;

    Hexagon *hex_p = &(it->second);

    // Return a pointer to the located hexagon.
    return hex_p;
}

void H3Grid::markNeighborBelow(Hexagon *h)
{
    CoordIJ c = h->neighborCoord(3);
    H3Hex *neighbor = H3Hex(c);
    neighbor->setDenseNeighbor(0);
    if (neighbor->dense() && !neighbor->possibleRoot())
        m_pos_roots.erase(neighbor);
}

void H3Grid::processPaths()
{
    // find cells with side 0 bordering empty space
    for (auto it = m_map.begin(); it != m_map.end(); ++it) {
        CoordIJ c = it->first;
        CoordIJ shift = edgeCoord(c, 0);
        if (m_map.find(shift) == m_map.end())
            m_possible.push_back(c);
    }

    m_min_i = m_possible[0].i;

    while (!m_possible.empty()) {
        findShape();
    }
    organizePaths();
}

void H3Grid::organizePaths()
{
    for (size_t i = 0; i < m_paths.size(); ++i) {
        H3Path *p = m_paths[i];
        parentOrChild(p);
        // Either add the path to the root list or the parent's list of
        // children.
        !p->parent() ?  roots.push_back(p) : p->parent()->addChild(p);
    }
    for (size_t i = 0; i < roots.size(); ++i) {
        roots[i]->finalize(H3CLOCKWISE);
    }

    // In the end, the list of paths is just the root paths.  Children can
    // be retrieved from their parents.
    m_paths = roots;
}

void H3Grid::findParentPath(H3Path *p)
{
    // Get an arbitrary dense hexagon that borders our path at edge 0
    CoordIJ hex = p->rootHex();
    int i = hex.i;
    // Keep moving down i until we find more hexagons bordering paths.
    // Then assign ours as root or child based on the paths it passes through
    while (i >= m_min_i) {
        IJPathMap::iterator it = m_hex_paths.find(hex);
        if (it != m_hex_paths.end()) {
            H3Path *parentPath = it->second;
            if (parentPath == p->parent()) {
                p->setParent(NULL);
            } 
            else if (!p->parent() && parentPath != p) {
                p->setParent(parentPath);
            }
        }
        hex = edgeCoord(hex, 3);
        i = hex.i;
    }
}

// Walk the outside of the hexagons to make a path.  
// Relative to H3 IJ coordinates, hexagon sides are labeled:
//
//               (+ I)
//                __0_
// (+ I, + J)  5 /    \ 1  (- J)
//              /      \
//              \      /
//      (+ J)  4 \____/ 2   (- I, - J)
//                  3
//               (- I)
//
void H3Grid::findShape()
{
    int edge(0);
    CoordIJ cur = m_possible[0];
    const CoordIJ orig = cur;
    H3Path *p = new H3Path(this, H3CLOCKWISE, orig);

    do {
        if (edge == 0) {
            std::vector<CoordIJ>::iterator it = std::find(m_possible.begin(), 
                                                      m_possible.end(), cur);
            if (it != m_possible.end()) 
                m_possible.erase(it);
        }
        addEdge(p, cur, edge);

        CoordIJ next = nextCoord(cur, edge);
        // if next is dense: go left
        if (m_map.find(next) != m_map.end()) {
            cur = next;
            m_min_i = std::min(m_min_i, cur.i - 1);
            edge--;
            if (edge < 0)
                edge = 5;
        }
        // if next not dense: keep moving around our current hex
        else {
            if (edge == 5)
                edge = 0;
            else
                edge++;
        }
    } while (!(cur == orig && edge == 0));
    m_paths.push_back(p);
}

void H3Grid::addEdge(H3Path * p, CoordIJ idx, int edge)
{    
    // get H3 directed edge of our current hexagon at a given edge
    H3Index src = ij2h3(idx);
    CoordIJ next_ij = edgeCoord(idx, edge);
    DirEdge dirEdge;

    if (cellsToDirectedEdge(src, ij2h3(next_ij), &dirEdge) != E_SUCCESS)
        throw hexer_error("Couldn't get directed edge.");
    p->push_back(dirEdge); 

    // if edge is 0 or 3, add our edge to the list of paths so it can be used
    // to find parent and child paths
    if (edge == 0)
        m_hex_paths.insert({idx, p});
    // if edge == 3: we need to normalize this so it can be processed as another
    // hexagon. Adds the edge as edge 0 of neighbor
    else if (edge == 3)
        m_hex_paths.insert({next_ij, p});
}

void H3Grid::processSample()
{
    if (m_res > 0 || m_sample.empty())
        return;
    double height = computeHexSize(m_sample, m_dense_limit);

    // bins for H3 auto resolution:
    // - H3 level ~roughly~ equivalent to hexer hexagon size at same edge value
    //     - (since our coords are in degrees, the appropriate values will vary based on
    //       location. Some way of scaling this by latitude would be more accurate)
    // - does not automatically make very large (>1km^2) or very small (<6m^2) hexagons
    // We ignore resolutions 1 through 7, so add 8 to the entry we find..
    const std::array<double, 7> resHeights { 2.0, 2.62e-4, 6.28e-5, 2.09e-5, 8.73e-6,
                                             3.32e-6, 1.4e-6 };

    for (int i = 0; i < resHeights.size(); ++i) {
        if (height < resHeights[i]) {
            m_res = i + 8;
        }
    }
    if (m_res == -1)
        throw hexer_error("unable to calculate H3 grid size!");
    
    for (auto pi = m_sample.begin(); pi != m_sample.end(); ++pi) {
        LatLng ll = *pi;
        addLatLng(&ll);
    }
    m_sample.clear();
    std::cout << "H3 resolution: " << m_res << std::endl; 
}

} // namespace hexer