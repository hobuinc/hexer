#include <hexer/Path.hpp>

namespace hexer
{

void Path::toWKT(std::ostream& output) const
{
    auto outputPoint = [&output](const Point& p) -> void
    {
        output << p.m_x << " " << p.m_y;
    };

    output << "(";

    auto pi = m_points.begin();
    if (pi != m_points.end())
        outputPoint(*pi++);
    for (; pi != m_points.end(); ++pi)
    {
        output << ", ";
        outputPoint(*pi);
    }

    output << ")";

    std::vector<Path *> paths = subPaths();
    for (auto p : paths)
    {
        output <<",";
        p->toWKT(output);
    }
}
} // namespace hexer
