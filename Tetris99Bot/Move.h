#pragma once
#include "piece.h"
#include "Value.h"
class M {
public:
	Piece * piece;
	int y;
	int x;
	int rotateIndex;
	bool isUseHold;
	M(Piece* piece, int r,int x, int y, bool isUseHold);
	M();
};

class Move {
public:
	M m;
	Value value;
	Move* next;
	Move();
	Move(int r, int x, int y, Piece* piece, Value value, bool isUseHold);
	Move(int r, int x, int y, Piece *piece, Value value);
	int getAllMoveValue()const;
	bool operator <(const Move &s) const;
};
