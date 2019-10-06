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
int correction(Board& board, Board& expected);
void filling(long board[20], long expected[20], int startX, int expectedHight);
bool ready(Piece** nextPieces);
bool notEqual(Piece** expected, Piece** now, int num);

static bool ready(Piece** nextPieces) {
	for (int i = 0; i < 6; i++) {
		if (nextPieces[i] == NULL)
			return false;
	}
	return true;
}
FrameTrack frameTracker = FrameTrack();
inline bool notEqual(Piece** expected, Piece** now, int num)
{
	for (int i = 0; i < num; i++) {
		if (expected[i] != NULL && !(now[i] == expected[i]))
			return true;
	}
	return false;
}
inline bool allEqual(Piece** expected, Piece** now) {

	for (int i = 0; i < 4; i++) {
		if (i < 4 && now[i + 1] != expected[i])
			return false;
	}
	return true;
}

int startFrameTracker(void *opaque) {
	frameTracker.start();
	return 0;
}
// proController��ͼ����
SDL_Rect rectA = { 719, 156, 60, 60 };
SDL_Rect rectX = { 650, 96, 60, 60 };
SDL_Rect rectZL = { 208, 2, 60, 40 };
SDL_Rect rectL = { 227, 284, 60, 50 };
SDL_Rect rectR = { 325, 284, 60, 50 };
SDL_Rect rectU = { 282, 230, 60, 50 };
#define dpad_N  8*9
int controllerState = dpad_N;

// �뵥Ƭ���Ĵ���ͨ�ŵļ��Ա��� = dpad*9+button
#define dpad_L  6*9
#define dpad_R  2*9
#define dpad_U  0*9
#define button_Y  1
#define button_B  2
#define button_A  3
#define button_X  4
#define button_L  5

int showProController(void *opaque)
{
	bool quit = false;
	SDL_Event event;

	//SDL��ʼ��������ֻ��ʾͼƬ������ֻ��ʼ��VIDEOϵͳ�������֧�ֲ鿴�ٷ��ĵ�
	SDL_Init(SDL_INIT_VIDEO);
	//Ϊ����ʾpngͼƬ������ʹ����ͼƬ�⣬����Ҫ������ʼ��
	IMG_Init(IMG_INIT_JPG);

	//����SDL����
	SDL_Window * window = SDL_CreateWindow("ProControler",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 889, 618, 0);
	//��Ⱦ��
	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
	//���ֻ����ʾһ��bmpͼƬ��ʹ��sdl���õĹ��ܼ���
	//SDL_Surface * image = SDL_LoadBMP("only_support_BMP.bmp");
	//��ΪҪ��ʾpngͼƬ������ʹ�����ⲿ�⣬sdl_image�⵱ǰ֧��jpg/png/webp/tiffͼƬ��ʽ

	// TODO �������·����
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

		int buttom = controllerState % 9;
		int dpad = controllerState - buttom;


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

// �˿�
CComm cComm;
inline void send(int state) {
	cComm.WriteByte(state);
	controllerState = state;
}

inline void wait(int t) {
	int time = clock();
	while (clock() - time < t) {
		continue;
	}
}

int rs[15];
int i = 0;
//das ��û��ʲô�ã�t99���ӳ�̫�󣬲���һ������һ�������İ�
int das[] = { 0,30,60,90,296,370 };
inline void exec(Move move) {
	i = 0;
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

	if (abs(moveY) == 5) {

		int mTime = das[abs(moveY)];
		int rTime = abs(rotate) * 58;
		int r = mTime - rTime - 58;

		if (move.m.isUseHold) {
			send(dpad_N + button_L);
			wait(29);
			send(dpad_N);
			wait(35);
		}
		int direction = moveY > 0 ? dpad_R : dpad_L;
		int rotateDirc = rotate > 0 ? button_A : button_X;
		send(direction);
		wait(r);
		for (int i = 0; i < abs(rotate); i++) {
			send(direction + rotateDirc);
			wait(29);
			send(direction);
			wait(29);
		}
		send(direction);
		wait(58);
		send(dpad_U);
		wait(29);
		send(dpad_N);
		wait(29);
		//cout << clock() << " dsl end " << abs(moveY)<< endl;
		return;


	}
	//

	if (abs(rotate) > abs(moveY)) {

		for (int k = 0; k < abs(rotate); k++) {
			if (k < abs(rotate) - abs(moveY)) {
				rs[i++] = dpad_N + (rotate > 0 ? button_A : button_X);
			}
			else
			{
				rs[i++] = (moveY > 0 ? dpad_R : dpad_L) + (rotate > 0 ? button_A : button_X);
			}
		}
	}
	else
	{
		for (int k = 0; k < abs(moveY); k++) {
			if (k < abs(moveY) - abs(rotate)) {
				rs[i++] = moveY > 0 ? dpad_R : dpad_L;
			}
			else
			{
				rs[i++] = (moveY > 0 ? dpad_R : dpad_L) + (rotate > 0 ? button_A : button_X);
			}
		}
	}
	rs[i++] = 0;
	rs[i++] = 72;

	if (move.m.isUseHold) {
		send(dpad_N + button_L);
		wait(29);
		send(dpad_N);
		wait(35);
	}
	for (int j = 0; j < i; j++) {
		send(rs[j]);
		wait(29);
		send(dpad_N);
		wait(29);
	}
	//cout << clock() << " end " << endl;

	//cout << endl;
}

// y b a x l
// 1 2 3 4 5
// 0 U 
// 2 R 
// 4 D
// 6 L
inline void pressA(int time = 24) {

	send(dpad_N + button_A);
	Sleep(time);
	send(dpad_N);
	Sleep(24);
}inline void pressB(int time = 24) {

	send(dpad_N + button_B);
	Sleep(time);
	send(dpad_N);
	Sleep(24);
}

void openComm() {

	// �˿ں�
	cComm.OpenComm("COM12");
	DWORD BaudRate = CBR_19200;
	BYTE ByteSize = 8;
	DWORD fParity = 1;
	BYTE  Parity = 'N';
	BYTE  StopBits = 1;
	cComm.ConfigureComm(BaudRate, ByteSize, Parity, StopBits);

}

inline bool allGarbage(long rows[20], int unIdentify) {

	for (int x = unIdentify - 1; x >= 0; x--) {
		if (!isGarbage(rows[x])) {
			return false;
		}
	}
	return true;
}

time_t seconds;
bool cannotOnlyOne = true;
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
		if (time(NULL) - secondsStart > 80)
			return;
		frameTracker.getNextPieces(nextPieces);
		if (ready(nextPieces))
			break;
	}
	cout << "ready" << endl;
	//��һ�β鿴����
	Board board;
	Board tmp;
	//�������е��ж�Ԥ����һ���Ļ���
	Board nextBoard;
	frameTracker.getNextPieces(nextBoard.next);
	nextBoard.current = nextBoard.next[0];
	nextBoard.refreshNext();

	seconds = time(NULL);

	while (true) {

		// ��ȡnext
		frameTracker.getNextPieces(board.next);

		if (time(NULL) - seconds > 10)
			return;

		int nextPieceNum = 0;
		for (int i = 0; i < 3; i++) {
			if (board.next[i] != NULL)
			{
				nextPieceNum++;
			}
		}
		if (nextPieceNum == 0 || (cannotOnlyOne && nextPieceNum == 1)) {
			//std::cout << clock() << " get " << nextPieceNum <<" piece,������ȡ" << endl;
			continue;
		}

		//std::cout << clock() << " get " << nextPieceNum << " piece" << endl;
		if (notEqual(nextBoard.next, board.next, nextPieceNum)) {
			if (time(NULL) - seconds > 10)
				return;
			continue;
		}

		for (int i = 0; i < 6; i++) {
			if (board.next[i] == NULL) {
				board.next[i] = nextBoard.next[i];
			}
		}

		seconds = time(NULL);

		//std::cout << clock() << " next changed" << endl;
		frameTracker.getFilled(board.rows);
		int r = correction(board, nextBoard);
		if (r == -1)
			return;
		board.hold = nextBoard.hold;
		board.current = nextBoard.current;
		std::cout << clock() << " ͼ���ȡ���" << endl;
		Move* move = board.get(nextPieceNum);
		//std::cout << move->m.piece->character() << " "
			//<< move->m.rotateIndex << " " << move->m.y << endl;
		std::cout << clock() << " ����������" << endl;
		exec(*move);
		board.paintAll(*move);

		if (board.unIdentify > 0) {

			long gray[20];
			do {

				if (time(NULL) - seconds > 10)
					return;
				std::cout << clock() << " ���¼���δȷ������" << endl;
				frameTracker.getGrayFilled(gray);
			} while (!allGarbage(gray, board.unIdentify));

			for (int x = 0; x < board.unIdentify; x++) {
				board.rows[x] = gray[x];
			}

			board.unIdentify = 0;

			std::cout << clock() << " ���¼������" << endl;
			board.paint();
		}


		std::cout << clock() << " ���ִ�����" << endl;
		cannotOnlyOne = board.next[0] == board.next[1];

		board.useMove(*move);
		//�����ռ��ʡʱ�俪��
		tmp = nextBoard;
		nextBoard = board;
		board = tmp;
		nextBoard.height = getHeight(nextBoard.rows);
		//std::cout << clock() << " һ�����ڽ���" << endl;

	}
}

int mainn(int argc, char* argv[]) {
	Board b = Board();

	b.current = new PieceL();
	b.hold = new PieceS();

	Move* m = b.get(0);

	return 0;


}
int main(int argc, char* argv[]) {
	av_log_set_level(AV_LOG_PANIC);
	Util::init();
	SDL_CreateThread(showProController, "ShowProController Thread", NULL);
	SDL_CreateThread(startFrameTracker, "FrameTracker Thread", NULL);
	openComm();
	Sleep(5000);
	while (true) {
		cout << "start" << endl;
		run();
		cout << "end" << endl;
	}
	return 0;
}

inline int getHeight(long* filled) {
	int h = -1;
	for (int x = 19; x >= 0; x--) {
		if (filled[x] != Board::EMPTY_ROW) {
			h = x;
			break;
		}
	}

	return h + 1;
}
inline int andNum(long n) {
	int count = 0;
	while (n != 0) {
		n = (n - 1) & n;
		count++;
	}
	return count;
}
inline bool isGarbage(long row) {
	return andNum(row) == 9;
}


inline bool hasBlock(long rows[10], int y) {
	for (int x = 0; x < 20; x++) {
		if (rows[x] && 1 << y != 0)
			return true;
	}
	return false;
}

inline int correction(Board &board, Board &expected) {

	// TODO ���startX��ȡ����ȷ������Ҫ
	int startX = 0;
	int maxSim = -1;
	if (expected.height > 0) {

		cout << clock() << " ��ǰ�߶ȣ�" << expected.height << endl;
		for (int x = 0; x < 20 - expected.height; x++) {
			int match = 0;
			for (int xx = 0; xx < expected.height; xx++) {
				match += andNum(~(expected.rows[xx] ^ board.rows[xx + x]));
			}
			if (match > maxSim) {
				maxSim = match;
				startX = x;
			}
		}
	}

	// startX�������ĸ߶�
	if (startX > 0) {
		cout << clock() << " �߶�������" << startX << endl;

		long gray[20];
		frameTracker.getGrayFilled(gray);

		board.unIdentify = 0;
		bool canBreak = true;
		int x = startX - 1;

		cout << clock() << " ��ǰ����Ҫʶ���ж��Ƿ�ɽ���" << x << endl;
		if (isGarbage(gray[x])) {
			cout << clock() << " ��ǰ����ȫ��ʶ���ж��Ƿ�ɽ���" << x << endl;
			for (int y = 0; y < 10; y++) {
				//ֻҪ������û�з������һ��֮ǰҲû�з���
				if (gray[x] && 1 << y == 0 && !hasBlock(expected.rows, y))
					canBreak = false;
			}
			if (canBreak) {
				board.unIdentify = x;
			}
			else {
				board.unIdentify = startX;
			}
			cout << clock() << " δȷ�������� " << board.unIdentify << endl;

		}
		else
		{
			cout << clock() << " ��ǰ��δȫ��ʶ��" << x << endl;
			board.unIdentify = startX;
			canBreak = false;
		}

		for (int x = 0; x < startX; x++) {
			if (x < board.unIdentify) {
				board.rows[x] = Board::EMPTY_ROW;
			}
			else
			{
				board.rows[x] = gray[x];
			}
		}
	}
	filling(board.rows, expected.rows, startX, expected.height);
	return 0;
}
inline void filling(long board[20], long expected[20], int startX, int expectedHight) {
	for (int x = startX; x < 20; x++) {
		if (x - startX < expectedHight)
			board[x] = expected[x - startX];
		else
			board[x] = Board::EMPTY_ROW;
	}
}