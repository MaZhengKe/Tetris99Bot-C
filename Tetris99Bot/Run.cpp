#include "FrameTrack.h"
#include "Util.h"
#include "piece.h"
#include "Board.h"
#include <windows.h>
#include "Comm.h"
#include <SDL.h>        
#include <SDL_image.h>

int main(int argc, char * argv[]);

int getHeight(long* filled);
int andNum(long n);
bool isGarbage(long row);
int correction(long* board, long* expected, int numToRefresh);
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
	for (int i = 0; i < 4; i++) {
		if  ( !(now[i] == expected[i]))
			return true;
	}
	return false;
}
bool allEqual(Piece** expected, Piece** now) {

	for (int i = 0; i < 4; i++) {
		if  (i<4 && now[i+1] != expected[i])
			return false;
	}
	return true;
}

int aa(void *opaque) {
	frameTracker = FrameTrack();
	frameTracker.startR();
	return 0;
}

#define EVENT_B_A  (SDL_USEREVENT + 3)
#define EVENT_NONE  (SDL_USEREVENT + 4)
SDL_Rect rectA = { 719, 156, 60, 60 };
SDL_Rect rectX = { 650, 96, 60, 60 };
SDL_Rect rectZL = { 208, 2, 60, 40 };
SDL_Rect rectL = { 227, 284, 60, 50 };
SDL_Rect rectR = { 325, 284, 60, 50 };
SDL_Rect rectU = { 282, 230, 60, 50 };
#define dpad_N  8*9
int bbb = dpad_N;

#define dpad_L  6*9
#define dpad_R  2*9
#define dpad_U  0*9
#define button_Y  1
#define button_B  2
#define button_A  3
#define button_X  4
#define button_L  5
bool a = false;

int createProController(void *opaque)
{
	bool quit = false;
	SDL_Event event;

	//SDL��ʼ��������ֻ��ʾͼƬ������ֻ��ʼ��VIDEOϵͳ�������֧�ֲ鿴�ٷ��ĵ�
	SDL_Init(SDL_INIT_VIDEO);
	//Ϊ����ʾpngͼƬ������ʹ����ͼƬ�⣬����Ҫ������ʼ��
	IMG_Init(IMG_INIT_JPG);

	//����SDL����
	SDL_Window * window = SDL_CreateWindow("SDL2 Displaying Image",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 889, 618, 0);
	//��Ⱦ��
	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
	//���ֻ����ʾһ��bmpͼƬ��ʹ��sdl���õĹ��ܼ���
	//SDL_Surface * image = SDL_LoadBMP("only_support_BMP.bmp");
	//��ΪҪ��ʾpngͼƬ������ʹ�����ⲿ�⣬sdl_image�⵱ǰ֧��jpg/png/webp/tiffͼƬ��ʽ
	SDL_Surface * image = IMG_Load("D:/99bot/switch.png");
	//�����ͼƬ����SDL��ͼ����
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_Rect rectA = { 719, 156, 60, 60 };

	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
	while (!quit)
	{//����Ϣѭ��
		SDL_WaitEvent(&event);

		switch (event.type)
		{   //�û��Ӳ˵�Ҫ���˳�����
		case SDL_QUIT:
			quit = true;
			break;
		}

		int buttom = bbb % 9;
		int dpad = bbb - buttom;


		//���ָ����ʾλ��ʹ������ע������������
		 //SDL_Rect dstrect = { 5, 5, 320, 240 };
		 //SDL_RenderCopy(renderer, texture, NULL, &dstrect);
		 //����ͼ���ʸ��Ƶ���Ⱦ��
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		switch (dpad) {
		case dpad_U:
			SDL_RenderFillRect(renderer, &rectU);
			break;
		case dpad_L:
			SDL_RenderFillRect(renderer, &rectL);
			break;
		case dpad_R:
			SDL_RenderFillRect(renderer, &rectR);
			break;
		}
		switch (buttom) {
		case button_A:
			SDL_RenderFillRect(renderer, &rectA);
			break;
		case button_X:
			SDL_RenderFillRect(renderer, &rectX);
			break;
		case button_L:
			SDL_RenderFillRect(renderer, &rectZL);
			break;
		}

		//��ʾ����
		SDL_RenderPresent(renderer);
	}
	//���͵������νṹ�����������Դ
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	//�˳�image��
	IMG_Quit();
	//�˳�SDL
	SDL_Quit();

	return 0;
}






CComm cComm;

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
		moveY = y - startY;
	}

	int max = max(abs(rotate), abs(moveY));
	for (int k = 0; k < max; k++) {
		if (k < abs(rotate) && k < abs(moveY)) {
			rs[i++] = (moveY > 0 ? dpad_R : dpad_L) + (rotate > 0 ? button_A : button_X);
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
		//cout << rs[j] << endl;
		cComm.WriteByte(rs[j]);
		bbb = rs[j];
		Sleep(32);
		cComm.WriteByte(dpad_N);
		bbb = dpad_N;
		Sleep(28);
	}
	cout << clock() <<" end "<< endl;
	//cout << endl;
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

void openComm() {

	cComm.OpenComm("COM8");
	DWORD BaudRate = CBR_19200;
	BYTE ByteSize = 8;
	DWORD fParity = 1;
	BYTE  Parity = 'N';
	BYTE  StopBits = 1;
	cComm.ConfigureComm(BaudRate, ByteSize, Parity, StopBits);

}

time_t seconds;

void run() {
	pressA();
	pressB();
	pressA();
	pressA();
	pressA(1000);

	Piece* nextPieces[6];

	time_t secondsStart;
	secondsStart = time(NULL);

	while (true) {
		if (time(NULL) - secondsStart > 60)
			return;
		frameTracker.getNextPieces(nextPieces);
		if (ready(nextPieces))
			break;
	}
	cout << "ready" << endl;
	//��һ�β鿴����
	Board board = getBoard();
	Board tmp;
	//�������е��ж�Ԥ����һ���Ļ���
	Board nextBoard;
	frameTracker.getNextPieces(nextBoard.next);
	nextBoard.currentPiece = nextBoard.next[0];
	nextBoard.refreshNext();

	seconds = time(NULL);

	while (true) {

		frameTracker.getNextPieces(board.next);
		cout << clock() << " ";
		bool haveNull = false;


		for (int i = 0; i < 6; i++) {
			if (board.next[i] == NULL) {
				cout << "�пյ�next"<< endl;
				haveNull = true;
				break;
			}
			cout <<  string(1, board.next[i]->character());
		}
		cout << endl;

		if (haveNull) {
			continue;
		}

		 if (notEqual(nextBoard.next, board.next)) {
			if (time(NULL) - seconds > 60)
				return;
			seconds = time(NULL);
			continue;
		}

		std::cout << clock() << " next changed" << endl;

		frameTracker.getFilled(board.rows);
		std::cout << clock() << " һ��rows��ȡ���" << endl;
		int r = correction(board.rows, nextBoard.rows, 5);
		if (r == -1)
			return;

		board.hold = nextBoard.hold;
		board.currentPiece = nextBoard.currentPiece;
		board.paint();
		//std::cout << clock() << " ͼ���ȡ���" << endl;
		Move* move = board.get(5);
		if (!move) {
			move = board.backGet();
		}
		board.paintAll(*move);
		std::cout << move->m.piece->character() << " "
			<< move->m.rotateIndex << " " << move->m.y << endl;
		//std::cout << clock() << " ����������" << endl;
		exec(*move);
		//std::cout << clock() << " ���ִ�����" << endl;

		 board.useMove(*move);
		//�����ռ��ʡʱ�俪��
		tmp = nextBoard;
		nextBoard = board;
		board = tmp;
		//std::cout << clock() << " һ�����ڽ���" << endl;
		
	}
}
int main(int argc, char* argv[]) {
	av_log_set_level(AV_LOG_PANIC);
	Util::init();
	SDL_Thread *pro_id = SDL_CreateThread(createProController, "Pro Thread", NULL);
	SDL_Thread *video_tid = SDL_CreateThread(aa, "Get Thread", NULL);
	openComm();
	Sleep(5000);
	while (true) {
		cout << "start" << endl;
		run();
		cout << "end" << endl;
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
int correction(long* board, long* expected, int numToRefresh) {

	if (time(NULL) - seconds > 60)
		return -1;
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
		cout << "�жϲ��������»�ȡ" << endl;
		frameTracker.getFilled(board);
		return correction(board, expected, numToRefresh - 1);
	}

	if (startX > 0) {
		cout << "�߶�������" << startX << endl;
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
	return 0;
}
void filling(long* board, long* expected, int startX, int expectedHight) {
	for (int x = startX; x < 20; x++) {
		if (x - startX < expectedHight)
			board[x] = expected[x - startX];
		else
			board[x] = Board::EMPTY_ROW;
	}
}