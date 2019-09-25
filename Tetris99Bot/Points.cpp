#include "Point.h"

using namespace mk;
Point::Point(int x, int y) :x(x), y(y)
{
}

bool Point::operator == (const Point& d) const {
	return (x == d.x && y == d.y);
}
