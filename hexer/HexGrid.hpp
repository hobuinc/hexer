#pragma once

#include "BaseGrid.hpp"

namespace hexer
{

static const double SQRT_3 = 1.732050808;

class HexGrid : public BaseGrid
{
public:
    HexGrid(double height, int denseLimit) : BaseGrid(denseLimit)
        { processHeight(height); }
    HexGrid(int denseLimit) : BaseGrid(denseLimit), m_width{-1.0}
        {}


    void addXY(double& x, double& y)
        { 
          Point p{x, y};
          addPoint(p);        
        }
    bool sampling() const
        { return m_width < 0; }
    Point offset(int idx) const
        { return m_offsets[idx]; }
    bool inGrid(int i)
        { return i >= m_minY; }
    Point findPoint(Segment& s);

private:
    void processHeight(double height);
    HexId findHexagon(Point p);
    HexId edgeHex(HexId hex, int edge) const;
    void parentOrChild(Path& p);

    /// @brief Height of the hexagons in the grid (2x apothem)
    double m_height;
    /// @brief Width of the hexagons in the grid
    double m_width;
    /// @brief Minimum y - 1.
    int m_minY;
    /// @brief Offsets of vertices of hexagon, going anti-clockwise from upper-left
    Point m_offsets[6];
    /// @brief Offset of the center of the hexagons.
    Point m_centerOffset;
    /// @brief Origin of the hex grid in point coordinates
    Point m_origin;

};

} // namespace hexer
