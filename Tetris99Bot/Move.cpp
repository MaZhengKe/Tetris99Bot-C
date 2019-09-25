#include "Move.h"
M::M(Piece* piece, int y, int rotateIndex, bool isUseHold) {
	this->piece = piece;
	this->y = y;
	this->rotateIndex = rotateIndex;
	this->isUseHold = isUseHold;
}
M::M() {}

Move::Move() {
}

Move::Move(int y, int rotateIndex, Piece* piece, Value value, bool isUseHold) {
	this->m = M(piece, y, rotateIndex, isUseHold);
	this->value = value;
}

Move::Move(int y, int rotateIndex, Piece *piece, Value value) {
	this->m = M(piece, y, rotateIndex, false);
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