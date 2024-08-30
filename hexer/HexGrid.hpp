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

    bool sampling() const
        { return m_width < 0; }
    Point offset(int idx)
        { return m_offsets[idx]; }

private:
    void processHeight(double height);
    HexId findHexagon(Point p);
    HexId edgeHex(HexId hex, int edge) const;
    void parentOrChild(Path p);
    Point findPoint(Segment s);

    double m_height;
    double m_width;
    int m_minY;
    Point m_offsets[6];
    Point m_centerOffset;
    Point m_origin;

};

} // namespace hexer
