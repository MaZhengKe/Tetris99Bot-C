#include "piece.h"

void Piece::init() {
	mat = cv::imread(Const::PATH + "/small/" + character() + ".png");
	bigMat = cv::imread(Const::PATH + "/big/" + character() + ".png");
	pieceShapes = shapes();
	rEndIndex = rotationsEndIndex();

	for (int r = 0; r <= rEndIndex; r++) {
		PieceShape* pieceShape = pieceShapes[r];
		for (int x = 0; x < 20; x++)
			for (int y = 0; y <= 10 - pieceShape->w; y++)
			{
				uint16_t* rows = shape[r][x][y];
				uint16_t* canS = canStop[r][x][y];
				Point *shapePoints = pieceShape->points;
				bool isCreateHole = false;
				for (int i = 0; i < 4; i++) {
					Point shapePoint = shapePoints[i];
					int xx = x - shapePoint.x;
					int yy = y + shapePoint.y;
					if(xx>=0)
						rows[xx] |= 1 << yy;
					if (xx > 0) {
						canS[xx-1] |= 1 << yy;
					}

					rows[xx] &= 0x3ff;
				}
			}

	}
}

int Piece::hashCode() {
	return number();
}

char  Piece::character() {
	return 'N';
}

int  PieceI::number() {
	return 2;
}

char  PieceI::character() {
	return 'I';
}

PieceShape**  PieceI::shapes() {
	PieceShape** a = new PieceShape*[2]{
			new PieceShape(4,1,3, 0,  new Point[4]{Point(0, 0),  Point(0, 1),  Point(0, 2),  Point(0, 3)}),
			new PieceShape(1,4,4, 1,   new Point[4]{Point(0, 0),  Point(1, 0),  Point(2, 0),  Point(3, 0)})
	};
	return a;
}


int  PieceJ::number() {
	return 6;
}


char PieceJ::character() {
	return 'J';
}


PieceShape** PieceJ::shapes() {
	return new PieceShape*[4]{
			new PieceShape(3,2,3, 0,  Point(0, 0),  Point(1, 0),  Point(1, 1),  Point(1, 2)),
			new PieceShape(2,3,3, 1,  Point(2, 0),  Point(0, 1),  Point(1, 1),  Point(2, 1)),
			new PieceShape(3,2,3, 2,  Point(0, 0),  Point(0, 1),  Point(0, 2),  Point(1, 2)),
			new PieceShape(2,3,4, 3,  Point(0, 0),  Point(1, 0),  Point(2, 0),  Point(0, 1))
	};
}

int  PieceL::number() {
	return 5;
}


char  PieceL::character() {
	return 'L';
}


PieceShape**  PieceL::shapes() {
	return new PieceShape*[4]{

			new PieceShape(3,2,3, 0,  Point(1, 0),  Point(1, 1),  Point(0, 2),  Point(1, 2)),
			new PieceShape(2,3,3, 1,  Point(0, 0),  Point(0, 1),  Point(1, 1),  Point(2, 1)),
			new PieceShape(3,2,3, 2,  Point(0, 0),  Point(1, 0),  Point(0, 1),  Point(0, 2)),
			new PieceShape(2,3,4, 3,  Point(0, 0),  Point(1, 0),  Point(2, 0),  Point(2, 1))
	};
}


int  PieceO::number() {
	return 1;
}


char PieceO::character() {
	return 'O';
}


PieceShape** PieceO::shapes() {
	return new PieceShape*[1]{
			new PieceShape(2,2,4, 0,  Point(0, 0),  Point(1, 0),  Point(0, 1),  Point(1, 1))
	};
}


int   PieceS::number() {
	return 3;
}


char  PieceS::character() {
	return 'S';
}


PieceShape**  PieceS::shapes() {
	return new PieceShape*[2]{
			new PieceShape(3,2,3, 0,  Point(1, 0),  Point(0, 1),  Point(1, 1),  Point(0, 2)),
			new PieceShape(2,3,3, 1,  Point(0, 0),  Point(1, 0),  Point(1, 1),  Point(2, 1))
	};
}

int  PieceT::number() {
	return 7;
}


char PieceT::character() {
	return 'T';
}


PieceShape** PieceT::shapes() {
	return new PieceShape*[4]{
			new PieceShape(3,2,3, 0,  Point(1, 0),  Point(0, 1),  Point(1, 1),  Point(1, 2)),
			new PieceShape(2,3,3, 1,  Point(1, 0),  Point(0, 1),  Point(1, 1),  Point(2, 1)),
			new PieceShape(3,2,3, 2,  Point(0, 0),  Point(0, 1),  Point(1, 1),  Point(0, 2)),
			new PieceShape(2,3,4, 3,  Point(0, 0),  Point(1, 0),  Point(2, 0),  Point(1, 1))
	};
}

int PieceZ::number() {
	return 4;
}


char PieceZ::character() {
	return 'Z';
}


PieceShape** PieceZ::shapes() {
	return new PieceShape*[2]{
			new PieceShape(3,2,3, 0,  Point(0, 0),  Point(0, 1),  Point(1, 1),  Point(1, 2)),
			new PieceShape(2,3,3, 1,  Point(1, 0),  Point(2, 0),  Point(0, 1),  Point(1, 1))
	};

}