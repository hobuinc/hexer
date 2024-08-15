#include <hexer/H3Path.hpp>

namespace hexer
{

std::vector<Point> H3Path::getPoints() const
{
    std::vector<Point> path_strip;
    if (m_direction == H3ANTICLOCKWISE) {
        for (int i = 0; i < m_segs.size(); ++i) {
            DirEdge edge = m_segs[i];
            CellBoundary edge_bound;

            if (directedEdgeToBoundary(edge, &edge_bound) != E_SUCCESS)
                throw hexer_error("unable to get cell boundary from directed edge!");
            double x = radsToDegs(edge_bound.verts[1].lng);
            double y = radsToDegs(edge_bound.verts[1].lat);
            path_strip.push_back(Point(x, y));
        }
    }
    else {
        for (size_t i = m_segs.size() - 1; i < m_segs.size(); --i) {
            DirEdge edge = m_segs[i];
            CellBoundary edge_bound;

            if (directedEdgeToBoundary(edge, &edge_bound) != E_SUCCESS)
                throw hexer_error("unable to get cell boundary from directed edge!");
            double x = radsToDegs(edge_bound.verts[1].lng);
            double y = radsToDegs(edge_bound.verts[1].lat);
            path_strip.push_back(Point(x, y));
    }
    }
    return path_strip;
}

} // namespace hexer
