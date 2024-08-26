#pragma once

#include "Hexagon.hpp"
#include "Mathpair.hpp"
#include "Path.hpp"
#include "Segment.hpp"
#include "export.hpp"

namespace hexer
{

class HEXER_DLL BaseGrid
{
public:
std::vector<Point> getSamples() const
    { return m_sample; }
void addPoint(Point p);
void findShapes();

private:
bool isDense(Hexagon *h);
void findShape(Hexagon *h);

int m_dense_limit;
std::vector<Point> m_sample;
};

} // namespace hexer