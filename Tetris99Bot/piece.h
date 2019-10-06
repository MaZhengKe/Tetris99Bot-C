#pragma once
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavutil/imgutils.h>
#include <SDL.h>
#include <SDL_main.h>
};
#include <iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include "PieceShape.h"
#include "Const.h"

class Piece
{
public:

	cv::Mat mat;
	cv::Mat bigMat;
	int rEndIndex;
	long shape[4][20][10][20];
	long canStop[4][20][10][20];
	PieceShape** pieceShapes;

	virtual int number() = 0;
	virtual PieceShape** shapes() = 0;
	virtual char character();
	virtual int rotationsEndIndex() =0;

	int hashCode();
	void init();
};

class PieceI :public Piece {
public:
	PieceI() {
		init();
	}
	int number();
	char character();
	PieceShape** shapes();
	int rotationsEndIndex() {
		return 1;
	}
};
class PieceJ :public  Piece {
public:
	PieceJ() {
		init();
	}
	int  number();
	char character();
	PieceShape** shapes();
	int rotationsEndIndex() {
		return 3;
	}
};
class PieceL :public Piece {
public:
	PieceL() {
		init();
	}
	int  number();
	char character();
	PieceShape** shapes();
	int rotationsEndIndex() {
		return 3;
	}
};
class PieceO : public Piece {
public:
	PieceO() {
		init();
	}
	int  number();
	char character();
	PieceShape** shapes();
	int rotationsEndIndex() {
		return 0;
	}
};
class PieceS :public Piece {
public:
	PieceS() {
		init();
	}
	int  number();
	char character();
	PieceShape** shapes();
	int rotationsEndIndex() {
		return 1;
	}
};
class PieceT :public Piece {
public:
	PieceT() {
		init();
	}
	int  number();
	char character();
	PieceShape** shapes();
	int rotationsEndIndex() {
		return 3;
	}
}
;
class PieceZ : public Piece {
public:
	PieceZ() {
		init();
	}
	int  number();
	char character();
	PieceShape** shapes();
	int rotationsEndIndex() {
		return 1;
	}
};
