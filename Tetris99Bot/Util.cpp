#include "Util.h"

list<Block> Util::includedMats = {};

Util::Util()
{
}


Util::~Util()
{
}
using namespace cv;
cv::Rect Util::pieceRects[20][10];

const Block Util::gray = Block(imread(Const::PIECE_PATH + "Gray.png"), "Dkblue", 7);

void Util::init() {

	Util::includedMats.push_back(Util::gray);
	Util::includedMats.push_back(Block(imread(Const::PIECE_PATH + "Dblue.png"), "Dkblue", 7));
	Util::includedMats.push_back(Block(imread(Const::PIECE_PATH + "Green.png"), "Green"));
	Util::includedMats.push_back(Block(imread(Const::PIECE_PATH + "Lblue.png"), "Ltblue"));
	Util::includedMats.push_back(Block(imread(Const::PIECE_PATH + "Purple.png"), "Purple"));
	Util::includedMats.push_back(Block(imread(Const::PIECE_PATH + "Red.png"), "Red   ", 11));
	Util::includedMats.push_back(Block(imread(Const::PIECE_PATH + "Yellow.png"), "Yellow"));
	Util::includedMats.push_back(Block(imread(Const::PIECE_PATH + "Orange.png"), "Orange"));


	int unit = 48;
	for (int x = 0; x < 20; x++) {
		for (int y = 0; y < 10; y++) {

			int rowStart = (unit * y);
			int colStart = ((19 - x) * unit);
			pieceRects[x][y] = Rect(rowStart, colStart, unit, unit);
		}
	}
}

static Piece * pieces[] = { new PieceO(), new PieceI(), new PieceS(), new PieceZ(), new PieceL(), new PieceJ(), new PieceT() };
int matSim(Mat mat, Mat subMat, int h, int w) {
	int match = 0;

	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			int diff = abs(mat.ptr(y * 4)[x * 3 * 4] - subMat.ptr(y * 4)[x * 3 * 4]) + abs(mat.ptr(y * 4)[x * 3 * 4 + 1] - subMat.ptr(y * 4)[x * 3 * 4 + 1]) + abs(mat.ptr(y * 4)[x * 3 * 4 + 2] - subMat.ptr(y * 4)[x * 3 * 4 + 2]);
			//log.info("{} {} {}", x, y, diff);
			if (diff < 100) {
				match++;
			}
		}
	}
	return match;
}


Piece* Util::bigMat2Piece(Mat mat) {
	Piece * candidate = NULL;
	int max = 300;
	for (Piece * piece : pieces) {
		int similarity = matSim(mat, piece->getBigMat(), 13, 26);
		//log.info("{} {}",piece,similarity);
		if (similarity > max) {
			max = similarity;
			candidate = piece;
		}
	}
	return candidate;
}

Piece* Util::mat2Piece(Mat mat) {
	Piece *candidate = NULL;
	int max = 200;
	for (Piece *piece : pieces) {
		Mat mat1 = piece->getMat();
		int similarity = matSim(mat, mat1, 11, 22);
		//log.info("{} {}",piece,similarity);
		if (similarity > max) {
			max = similarity;
			candidate = piece;
		}
	}
	return candidate;
}
Mat Util::toHSV(Mat img) {
	Mat imgHSV;
	cv::cvtColor(img, imgHSV, COLOR_BGR2HSV);
	return imgHSV;
}

double Util::similarity(Mat mat, Block block) {
	if (block.colour == gray.colour) {

		int all = 0;

		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				int num = abs(mat.ptr(y * 6)[x * 6 * 3 + 1] - block.hsv[x][y][1]) + abs(mat.ptr(y * 6)[x * 6 * 3 + 2] - block.hsv[x][y][2]);
				if (num < 35)
					all++;
			}
		}

		return all;
	}

	int all = 0;

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {

			if (mat.ptr(y * 6)[x * 6 * 3 + 2] < 85) {
				all -= 2;
				continue;
			}
			int num = abs(mat.ptr(y * 6)[x * 6 * 3] - block.hsv[x][y][0]);

			if (num < 20)
				all++;
		}
	}
	return all;
}

bool  Util::isIncludeMat(Mat mat) {
	for (Block excludedMat : Util::includedMats) {
		double similarity = Util::similarity(mat, excludedMat);
		if (similarity > 40)
			return true;
	}
	return false;
}

void Util::getFilled(long * rows, Mat mat, bool onlyUseGray) {
	if (onlyUseGray) {
		for (int x = 0; x < 20; x++) {
			rows[x] = 0;
			for (int y = 0; y < 10; y++) {
				Mat subMat = Mat(mat, pieceRects[x][y]);
				if (similarity(subMat, gray) > 40)
					rows[x] |= 1 << y;
			}
		}
	}
	else {
		for (int x = 0; x < 20; x++) {
			rows[x] = 0;
			for (int y = 0; y < 10; y++) {
				Mat subMat = Mat(mat, pieceRects[x][y]);
				if (Util::isIncludeMat(subMat)) {
					rows[x] |= 1 << y;
				}
			}
		}
	}
}
void Util::getFilled(long * rows, Mat mat, long * grayRows) {
	for (int x = 0; x < 20; x++) {
		rows[x] = 0;
		grayRows[x] = 0;
		for (int y = 0; y < 10; y++) {
			Mat subMat = Mat(mat, pieceRects[x][y]);
			if (Util::isIncludeMat(subMat)) {
				rows[x] |= 1 << y;
				if (similarity(subMat, gray) > 40)
					grayRows[x] |= 1 << y;
			}
		}
	}
}