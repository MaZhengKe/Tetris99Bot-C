#pragma once

#include "piece.h"
#include "Value.h"
#include "Move.h"

class Board {
public:
	static int s[];
	static long EMPTY_ROW;
	static long FULL_ROW;

	long rows[20];	
	Piece* next[6];
	Piece* hold;
	Piece* currentPiece;

	Board();
	Board(long rows[20], Piece* next[6], Piece* hold, Piece *currentPiece);
	Board copy();
	void setBits(int x, long pieceRowCells);
	void clearBits(int x, long inversePieceRowCells);
	bool isBitsFree(int x, long pieceRowCells);
	void fill(int x, int y);
	int fill(int y, PieceShape* shape);
	bool fill(int x, int y, PieceShape* shape);
	int minX(int y, PieceShape* shape);
	int clearRows(int x, int pieceHeight);
	Value value(int avgV);
	bool isFilled(int x, int y);
	bool isFree(int x, int y);
	void paintAll(Move move);
	bool contain(list<Point> points, int x, int y);
	void paint();
	Move * backGet();
	Move* get(int dep);
	void getMoves(set<Move> *moves);
	void backgetMoves(set<Move>* moves);
	void backgetPieceMoves(set<Move>* moves, Piece * piece, bool isUseHold);
	void getPieceMoves(set<Move>* moves, Piece* piece, bool isUseHold = false);
	void useMove(Move move);
	void refreshNext();
	void fill(Move move);
};

