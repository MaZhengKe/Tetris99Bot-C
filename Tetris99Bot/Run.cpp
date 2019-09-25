#include "FrameTrack.h"
#include "Util.h"
#include "piece.h"
#include "Board.h"
#include <windows.h>
#include "Comm.h"

int main(int argc, char * argv[]);

int getHeight(long* filled);
int andNum(long n);
bool isGarbage(long row);
long* correction(long* board, long* expected, int numToRefresh);
void filling(long* board, long* expected, int startX, int expectedHight);
static bool ready(Piece** nextPieces);
Board getBoard();
bool notEqual(Piece** expected, Piece** now);

static bool ready(Piece** nextPieces) {
	for (int i = 0; i < 6; i++) {
		if (nextPieces[i] == NULL)
			return false;
	}
	return true;
}
FrameTrack frameTracker;
Board getBoard() {
	Board board;
	frameTracker.getFilled(board.rows);
	frameTracker.getNextPieces(board.next);
	return board;
}
bool notEqual(Piece** expected, Piece** now) {
	for (int i = 0; i < 6; i++) {
		if (now[i] == NULL || (i < 4 && !(now[i] == expected[i])))
			return true;
	}
	return false;
}

int aa(void *opaque) {
	frameTracker.startR();
	return 0;
}
CComm cComm;

#define dpad_N  8*9
#define dpad_L  6*9
#define dpad_R  2*9
#define dpad_U  0*9
#define button_Y  1
#define button_B  2
#define button_A  3
#define button_X  4
#define button_L  5
void exec(Move move) {

	int rs[15];
	int i = 0;
	if (move.m.isUseHold) {
		rs[i++] = dpad_N + button_L;
	}
	int rotate = 0;
	int moveY = 0;

	int rotateIndex = move.m.rotateIndex;
	int endIndex = move.m.piece->rotationsEndIndex();
	int ni = endIndex + 1 - rotateIndex;
	PieceShape* pieceShape = move.m.piece->pieceShapes[move.m.rotateIndex];
	int startY = pieceShape->y;
	int y = move.m.y;

	if (endIndex == 1 && rotateIndex == 1 && y > startY) {
		rotate = 1;
		moveY = y - startY - 1;
	}
	else {
		if (ni < rotateIndex) {
			rotate = ni;
		}
		else {
			rotate = -rotateIndex;
		}
		moveY =  y-startY ;
	}

	int max = max(abs(rotate), abs(moveY));
	for (int k = 0; k < max; k++) {
		if (k < abs(rotate) && k < abs(moveY)) {
			rs[i++] = (moveY > 0 ? dpad_R : dpad_L) +( rotate > 0 ? button_A : button_X);
		}
		else if (k < abs(rotate)) {
			rs[i++] = dpad_N + (rotate > 0 ? button_A : button_X);
		}
		else {
			rs[i++] = moveY > 0 ? dpad_R : dpad_L;
		}
	}
	rs[i++] = 0;
	rs[i++] = 72;

	for (int j = 0; j < i; j++) {
		cout << rs[j] << endl;
		cComm.WriteByte(rs[j]);
		Sleep(32);
		cComm.WriteByte(dpad_N);
		Sleep(28);
	}
	cout << endl;
}

// y b a x l
// 1 2 3 4 5
// 2 R 
//6 L
//0 U 
//4 D
void pressA(int time = 24) {

	cComm.WriteByte(8 * 9 + 3);
	Sleep(time);
    cComm.WriteByte(8 * 9);
	Sleep(24);
}void pressB(int time = 24) {

	cComm.WriteByte(8 * 9 + 2);
	Sleep(time);
    cComm.WriteByte(8 * 9);
	Sleep(24);
}
int main(int argc, char* argv[]) {
	cComm.OpenComm("COM8");
	DWORD BaudRate = CBR_19200;
	BYTE ByteSize = 8;
	DWORD fParity = 1;
	BYTE  Parity = 'N';
	BYTE  StopBits = 1;
	//if (cComm.ConfigureComm(BaudRate, ByteSize, fParity, Parity, StopBits))
	cComm.ConfigureComm(BaudRate, ByteSize, Parity, StopBits); 
	pressB();
	pressA();
	pressA();
	pressA(1000);

	Util::init();
	frameTracker = FrameTrack();
	//std::thread t1([this] frameTracker.startR());

	SDL_Thread *video_tid = SDL_CreateThread(aa, "Get Thread", NULL);

	Sleep(10000);

	Piece* nextPieces[6];
	while (true) {
		frameTracker.getNextPieces(nextPieces);
		if (ready(nextPieces))
			break;
	}
	cout << "ready" << endl;
	//第一次查看画面
	Board board = getBoard();
	Board tmp;
	//依据现有的行动预测下一步的画面
	Board nextBoard;
	frameTracker.getNextPieces(nextBoard.next);
	nextBoard.currentPiece = nextBoard.next[0];
	nextBoard.refreshNext();

	while (true) {

		frameTracker.getNextPieces(board.next);
		if (notEqual(nextBoard.next, board.next)) {
			continue;
		}
		std::cout << clock() << " next changed" << endl;
		frameTracker.getFilled(board.rows);
		std::cout << clock() << " 一次rows获取完成" << endl;
		correction(board.rows, nextBoard.rows, 5);

		board.hold = nextBoard.hold;
		board.currentPiece = nextBoard.currentPiece;
		std::cout << clock() << " 图像获取完成" << endl;
		Move* move = board.get(5);
		if (!move) {
			move = board.backGet();
		}
		std::cout << clock() << " 结果计算完成" << endl;
		exec(*move);
		std::cout << clock() << " 结果执行完成" << endl;

		std::cout << move->m.piece->character() << " "
			<< move->m.rotateIndex << " "<< move->m.y << endl;
		board.paintAll(*move);
		board.useMove(*move);
		//交换空间节省时间开销
		tmp = nextBoard;
		nextBoard = board;
		board = tmp;
		std::cout << clock() << " 一次周期结束" << endl;
	}
	return 0;
}

int getHeight(long* filled) {
	int h = -1;
	for (int x = 19; x >= 0; x--) {
		if (filled[x] != Board::EMPTY_ROW) {
			h = x;
			break;
		}
	}

	return h + 1;
}
int andNum(long n) {
	int count = 0;
	while (n != 0) {
		n = (n - 1) & n;
		count++;
	}
	return count;
}
bool isGarbage(long row) {
	return andNum(row) == 9;
}
long* correction(long* board, long* expected, int numToRefresh) {

	if (numToRefresh < 0) {
		frameTracker.getFilled(board);
		numToRefresh = 5;
	}

	int height = getHeight(expected);

	int startX = 0;
	float maxSim = 0;
	bool hasEq = false;
	for (int x = 0; x < 20 - height; x++) {

		int all = 0;
		int match = 0;

		for (int xx = 0; xx < height; xx++) {
			match += andNum(~(expected[xx] ^ board[xx + x]));
			all += 10;
		}
		float f = (float)match / all;
		if (f == maxSim) {
			hasEq = true;
		}
		if (f > maxSim) {
			hasEq = false;
			maxSim = f;
			startX = x;
		}
	}

	if (hasEq) {
		cout << "判断不出，重新获取" << endl;
		frameTracker.getFilled(board);
		return correction(board, expected, numToRefresh - 1);
	}

	if (startX > 0) {
		cout << "高度上升：" <<startX<< endl;
		frameTracker.getFilled(board, true);
		for (int x = 0; x < startX; x++) {
			if (!isGarbage(board[x])) {
				frameTracker.getFilled(board, true);
				filling(board, expected, startX, height);
				return correction(board, expected, numToRefresh - 1);
			}
		}
	}
	else {
	}
	filling(board, expected, startX, height);
	return board;
}
void filling(long* board, long* expected, int startX, int expectedHight) {
	for (int x = startX; x < 20; x++) {
		if (x - startX < expectedHight)
			board[x] = expected[x - startX];
		else
			board[x] = Board::EMPTY_ROW;
	}
}