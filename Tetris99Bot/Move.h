#pragma once
#include "piece.h"
#include "Value.h"
class M {
public:
	Piece * piece;
	int y;
	int rotateIndex;
	bool isUseHold;
	M(Piece* piece, int y, int rotateIndex, bool isUseHold);
	M();
};

class Move {
public:
	M m;
	Value value;
	Move* next;
	Move();
	Move(int y, int rotateIndex, Piece* piece, Value value, bool isUseHold);
	Move(int y, int rotateIndex, Piece *piece, Value value);
	int getAllMoveValue()const;
	bool operator <(const Move &s) const;
};
