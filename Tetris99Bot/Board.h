#pragma once

#include "piece.h"
#include "Value.h"
#include "Move.h"


class Board {
public:
	static int clearScore[];
	static long EMPTY_ROW;
	static long FULL_ROW;

	// 地形
	long rows[20];

	// 不能确定的行数
	int unIdentify;

	// 高度
	int height;

	// next列表
	Piece* next[6];

	// hold
	Piece* hold;

	// 当前方块
	Piece* current;

	int cX;
	int cY;
	int cZ;



	Board();
	Board(long rows[20], Piece* next[6], Piece* hold, Piece *current, int unIdentify, int height);
	Board copy();
	void setBits(int x, long pieceRowCells);
	void clearBits(int x, long inversePieceRowCells);
	bool isBitsFree(int x, long pieceRowCells);
	bool fill(int r, int x, int y, Piece* piece);
	int minX(int y, int r, Piece* piece);
	int clearRows(int x, int pieceHeight);
	Move value(int y, int r, Piece* piece);
	Move backValue(int y, int r, Piece* piece);
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


