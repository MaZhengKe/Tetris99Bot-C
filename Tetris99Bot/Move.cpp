#include "Move.h"
M::M(Piece* piece, int r, int x,int y, bool isUseHold) {
	this->piece = piece;
	this->rotateIndex = r;
	this->x = x;
	this->y = y;
	this->isUseHold = isUseHold;
}
M::M() {}

Move::Move() {
}

Move::Move(int r, int x , int y, Piece* piece, Value value, bool isUseHold) {
	this->m = M(piece, r,x,y, isUseHold);
	this->value = value;
}

Move::Move(int r, int x, int y ,Piece *piece, Value value) {
	this->m = M(piece, r, x, y, false);
	this->value = value;
}

int Move::getAllMoveValue()const {
	int avg = this->value.avgV;
	int num = 1;
	const Move* current = this;
	while (current->next != NULL) {
		num++;
		avg += current->next->value.avgV;
		current = current->next;
	}
	return avg / num + current->value.sumV;
}

bool Move::operator <(const Move &s) const {
	return this->getAllMoveValue() > s.getAllMoveValue();
}