#include <hexer/BaseGrid.hpp>
#include <hexer/HexGrid.hpp>

namespace hexer
{

void HexGrid::processHeight(double height)
{
    m_maxSample = 10000;
    m_height = height;
    m_minY = 1;
    m_width = (3 / (2 * SQRT_3)) * m_height;
    m_offsets[0] = Point(0, 0);
    m_offsets[1] = Point(-m_width / 3, m_height / 2);
    m_offsets[2] = Point(0, m_height);
    m_offsets[3] = Point(2 * m_width / 3, m_height);
    m_offsets[4] = Point(m_width, m_height / 2);
    m_offsets[5] = Point(2 * m_width / 3, 0);
    m_centerOffset = Point(m_width / 3, m_height / 2);
}

HexId HexGrid::findHexagon(Point p)
{
    int x, y;

    if (m_counts.empty())
    {
        m_origin = p;
        return HexId{0,0};
    }

    // Offset by the origin.
    p -= m_origin;

    double col = p.m_x / m_width;

    // First calculate X and Y as if we had a bunch of offset rectangles.
    // This works for 2/3 of the width of the hexagons.
    x = (int)floor(col);
    if (x % 2 == 0)
        y = static_cast<int>(floor(p.m_y / m_height));
    else
        y = static_cast<int>(floor((p.m_y - (m_height / 2)) / m_height));

    // Compute the column remainder to determine if we are in a strip where
    // the hexagons overlap (the mini-column).
    double xcolOffset = col - floor(col);
    if (xcolOffset > 2.0/3.0)
    {
        // Calculate the xvalue as a fraction of the width of the column-piece
        // containing multiple hex columns.  These overlap columns are 1/3
        // the total width of any column.

        // Subtract the 2/3 of the value not relevant to the mini-column.
        xcolOffset -= 2.0/3.0;
        // Scale the value to the width of the mini-column.
        xcolOffset *= 3.0;

        // Each halfrow contains a single sloping edge of a hexagon.
        // The slope of the edge is either sqrt(3) or -sqrt(3).  The edge
        // extends from top left to lower right or from bottom left to top
        // right.  What we do here is compute the horizontal fraction of
        // the box (xcolOffset) and the vertical fraction of the box
        // (yrowOffset) and then compare them.
        double halfrow = p.m_y / (m_height / 2);
        int halfy = (int)halfrow;
        double yrowOffset = halfrow - floor(halfrow);

        // Negative slope case.
        if ((halfy % 2 == 0 && x % 2 == 0) || (x % 2 && halfy % 2))
        {
            if (xcolOffset > yrowOffset)
            {
                if (x % 2 == 0)
                    y--;
                x++;
            }
        }
        // Positive slope case.
        else
        {
            if (yrowOffset > xcolOffset)
            {
                if (x % 2)
                    y++;
                x++;
            }
        }
    }

    m_minY = std::min(m_minY, y - 1);

    return HexId{x, y};
}

HexId HexGrid::edgeHex(HexId hex, int edge) const
{
    const HexId even[] = {{0, -1}, {-1, -1}, {-1, 0}, {0, 1}, {1, 0}, {1, -1}};
    //static int evenx[] = { 0, -1, -1, 0, 1, 1 };
    //static int eveny[] = { -1, -1, 0, 1, 0, -1 };
    const HexId odd[] = {{0, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}};
    //static int oddx[] = { 0, -1, -1, 0, 1, 1 };
    //static int oddy[] = { -1, 0, 1, 1, 1, 0 };

    if (hex.i % 2 == 0) 
        return hex + even[edge];
    else 
        return hex + odd[edge];

}

void HexGrid::parentOrChild(Path p)
{
    // p.rootHex() might need tweaks to return a useful hexagon
    HexId hex = p.rootHex();
    // j (should be) equivalent to y
    int j = hex.j;
    while (j >= m_minY) {
        std::unordered_map<HexId, Path *>::iterator it = m_hexPaths.find(hex);
        if (it != m_hexPaths.end()) {
            Path *parentPath = it->second;
            if (parentPath == p.parent()) {
                p.setParent(NULL);
            } 
            else if (!p.parent() && parentPath != &p) {
                p.setParent(parentPath);
            }
        }
        --j;
        hex = HexId{hex.i, j}; 
    }
}

Point HexGrid::findPoint(Segment s)
{
    // inefficient. Re-calculates hex center position every time it's called
    HexId hex = s.hex;
    Point pos;

    pos.m_x = hex.i * m_width;
    pos.m_y = hex.j * m_height;
    if (hex.i % 2 != 0)
        pos.m_y += (m_height / 2);
    
    return pos + offset(s.edge) + m_origin;
}

} // namespace hexer