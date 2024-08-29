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
    return HexId{x, y};
}

HexId edgeHex(HexId hex, int edge)
{
    static int evenx[] = { 0, -1, -1, 0, 1, 1 };
    static int eveny[] = { -1, -1, 0, 1, 0, -1 };
    static int oddx[] = { 0, -1, -1, 0, 1, 1 };
    static int oddy[] = { -1, 0, 1, 1, 1, 0 };

    if (hex.i % 2 == 0) {
        hex.i += evenx[edge];
        hex.j += eveny[edge];
    }
    else {
        hex.i += oddx[edge];
        hex.j += oddy[edge];
    }

    return hex;
}

} // namespace hexer