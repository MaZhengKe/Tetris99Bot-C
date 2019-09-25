#include "PieceShape.h"

PieceShape::PieceShape(int y, Point * points)
{
	this->y = y;
	this->points = points;
}

PieceShape::PieceShape() {

}
PieceShape::PieceShape(int w, int h, int y, int rotateIndex, Point * points)
{
	this->w = w;
	this->h = h;
	this->y = y;
	this->rotateIndex = rotateIndex;
	this->points = points;
}

Point * PieceShape::getPoints()
{
	return points;
}

PieceShape::PieceShape(int y, Point point1, Point point2, Point point3, Point point4) 
: PieceShape(y, new Point[4]{ point1,  point2,  point3,  point4 })
{
}

PieceShape::PieceShape(int w, int h, int y, int rotateIndex, Point point1, Point point2, Point point3, Point point4) 
	:PieceShape(w, h, y, rotateIndex, new Point[4]{ point1,  point2,  point3,  point4 })
{
}