#include <hexer/H3Path.hpp>

namespace hexer
{

std::vector<Point> H3Path::getPoints() const
{
    std::vector<Point> path_strip;
    int idx;
    int reverse_size = m_segs.size() - 1;
    for (int i = 0; i < m_segs.size(); ++i) {
        if (m_direction == H3ANTICLOCKWISE)
            idx = i;
        else
            idx = reverse_size - i;

        DirEdge edge = m_segs[idx];
        CellBoundary edge_bound;

        if (directedEdgeToBoundary(edge, &edge_bound) != E_SUCCESS)
            throw hexer_error("unable to get cell boundary from directed edge!");
        double x = radsToDegs(edge_bound.verts[1].lng);
        double y = radsToDegs(edge_bound.verts[1].lat);
        path_strip.push_back(Point(x, y));
    }
    return path_strip;
}

} // namespace hexer
