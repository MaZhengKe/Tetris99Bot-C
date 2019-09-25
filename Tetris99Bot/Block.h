#pragma once

#include<opencv2/core/core.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc.hpp>

class Block {

public:
	cv::Mat mat;
	cv::String colour;
	int que;
	uchar hsv[8][8][3];

	Block();
	Block(cv::Mat mat, cv::String colour);
	Block(cv::Mat mat, cv::String colour, int que);
};