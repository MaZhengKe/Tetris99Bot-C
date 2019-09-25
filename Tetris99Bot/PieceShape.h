#pragma once
#include "Point.h"
using namespace mk;
class PieceShape {
public:
	Point *points;
	int y;
	int rotateIndex;
	int w;
	int h;

	PieceShape(int y, Point* points);
	PieceShape();
	Point* getPoints();
	PieceShape(int w, int h, int y, int rotateIndex, Point* points);
	PieceShape(int y, Point point1, Point point2, Point point3, Point point4);
	PieceShape(int w, int h, int y, int rotateIndex, Point point1, Point point2, Point point3, Point point4);

};

