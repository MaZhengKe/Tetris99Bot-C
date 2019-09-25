#pragma once
namespace mk{
	class Point {
	public:
		const int x;
		const int y;
		Point(int x, int y);
		bool operator == (const Point& d) const;
	};
}