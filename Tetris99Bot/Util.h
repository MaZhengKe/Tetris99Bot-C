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

#include<opencv2/core/core.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include "Logger.h"
#include <list>   
#include "Block.h"
#include "piece.h"
#include "Const.h"
class Util
{
public:
	Util();
	~Util();
	static void init();
	static cv::Rect pieceRects[20][10];
	static Piece* bigMat2Piece(cv::Mat mat);
	static Piece* mat2Piece(cv::Mat mat);
	static cv::Mat toHSV(cv::Mat img);
	static list<Block> includedMats;
	static const Block gray;
	static double similarity(cv::Mat mat, Block block);
	static bool isIncludeMat(cv::Mat mat);
	static void getFilled(uint16_t * rows, cv::Mat mat, bool onlyUseGray);
	static void getFilled(uint16_t * rows, cv::Mat mat, uint16_t * grayRows);
};