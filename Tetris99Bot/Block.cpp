#include "Block.h"
#include "Util.h"
#include <string.h>

using namespace cv;
Block::Block()
{
}

Block::Block(cv::Mat mat, cv::String colour) {
	new (this)Block(mat, colour, 20);
}

Block::Block(Mat mat, String colour, int que) {
		this->mat = Util::toHSV(mat);
		this->colour = colour;
		this->que = que;
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				uchar* data = &this->mat.ptr<uchar>(y*6)[x*6*3];
				memcpy(hsv[x][y], data, 3);
			}
		}
}