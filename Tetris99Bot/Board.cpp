#include "Board.h"

int Board::clearScore[] = { 0, -800, -800, -300, 6200 };
uint16_t Board::EMPTY_ROW = 0;
uint16_t Board::FULL_ROW = 0x3FF;
Board::Board() {}

inline Board::Board(uint16_t rows[20], Piece* next[6], Piece* hold, Piece *current, int unIdentify, int height) {
	memcpy(this->rows, rows, 20 * sizeof(uint16_t));
	memcpy(this->next, next, 6 * sizeof(Piece*));
	this->hold = hold;
	this->current = current;
	this->unIdentify = unIdentify;
	this->height = height;
}

inline Board Board::copy() {
	return  Board(rows, next, hold, current, unIdentify, height);
}

inline void Board::setBits(int x, uint16_t pieceRowCells) {
	rows[x] |= pieceRowCells;
}

inline void Board::clearBits(int x, uint16_t inversePieceRowCells) {
	rows[x] &= inversePieceRowCells;
}

inline bool Board::isBitsFree(int x, uint16_t pieceRowCells) {
	return (rows[x] & pieceRowCells) == 0;
}

inline  bool Board::fill(int r, int x, int y, Piece* piece) {
	int h = piece->pieceShapes[r]->h;
	for (int xx = x - h + 1; xx <= x; xx++) {
		rows[xx] |= piece->shape[r][x][y][xx];
	}

	uint16_t *canS = piece->canStop[r][x][y];
	for (int xx = x - h; xx < x && xx >= 0; xx++) {
		if ((rows[xx] & canS[xx]) != canS[xx]) {
			return true;
		}
	}
	return false;
}

inline int Board::minX(int y, int r, Piece* piece) {
	int h = piece->pieceShapes[r]->h;
	for (int x = 19; x >= h; x--) {
		uint16_t *canS = piece->canStop[r][x][y];
		for (int xx = x - h; xx < x; xx++) {
			if ((rows[xx] & canS[xx]) != 0) {
				return x;
			}
		}
	}
	return h - 1;
}

inline int Board::clearRows(int x, int pieceHeight) {
	int clearedRows = 0;
	int x1 = x - pieceHeight;

	// Find first row to clear
	do {
		x1++;
		if (rows[x1] == FULL_ROW) {
			clearedRows++;
		}
	} while (clearedRows == 0 && x1 < x);

	// Clear rows
	if (clearedRows > 0) {
		int x2 = x1;

		while (x1 < 20) {
			x2++;
			while (x2 <= x && rows[x2] == FULL_ROW) {
				clearedRows++;
				x2++;
			}
			if (x2 < 20) {
				rows[x1] = rows[x2];
			}
			else {
				rows[x1] = Board::EMPTY_ROW;
			}
			x1++;
		}
	}
	return clearedRows;
}


inline int calc_01_change_count(uint16_t n_input)
{
	n_input = n_input << 1;
	n_input |= 0b100000000011;

	uint16_t tmp = (n_input << 1);
	uint16_t n = n_input ^ tmp;//check how many bits are different
	uint16_t start = (n >> (12)), end = (n & 0x01);//check the first and the last bit
	uint16_t count = 0;//num of 1->0 or 0->1

	for (count = 0; n; ++count)
	{
		n &= (n - 1);
	}
	count -= (start + end);
	return count;
}

inline bool isWell(uint16_t l, int y) {
	l = l << 1;
	l |= 0b100000000011;

	l = l >> y;
	l &= 0b111;
	return l == 0b101;
}
inline bool isTwoWell(uint16_t l, int y) {
	l = l << 1;
	l |= 0b100000000011;

	l = l >> y;
	l &= 0b1111;
	return l == 0b1001;
}

int well[] = { 0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91, 105,
	120, 136, 153, 171, 190, 210 };
int twoWell[] = { 0, 0, 1, 10, 15, 21, 28, 28, 36, 45, 55, 66, 78, 91, 105,
	120, 136, 153, 171, 190, 210 };

inline Move Board::value(int y, int r, Piece* piece) {

	PieceShape* shape = piece->pieceShapes[r];
	int x = minX(y, r, piece);
	// 超高
	if (x == 20) {
		return   Move(r, x, y, piece, Value(-99999, -99999));
	}
	//不允许放置
	if (x - shape->h + 1 < unIdentify) {
		return   Move(r, x, y, piece, Value(-99999, -99999));
	}

	bool isCreateHole = fill(r, x, y, piece);

	int clean = clearRows(x, shape->h);
	//创建了新的洞且没有消除任何行
	if (isCreateHole && clean == 0) {
		return   Move(r, x, y, piece, Value(-99999, -99999));
	}

	int height = 2 * x + shape->h - 1;
	int rowTransitions = 0;
	int colTransitions = 0;
	int numberOfHoles = 0;
	int wellSums = 0;
	int twoWellSums = 0;
	bool free0 = true;

	for (int y = 0; y < 10; y++) {
		bool now = true;
		bool f = false;
		int block = 0;

		for (int x = 19; x >= 0; x--) {
			bool isFill = isFilled(x, y);
			if (isFill != now) {
				now = isFill;
				rowTransitions++;
			}
			if (isFill) {

				if (y == 0) {
					free0 = false;
				}
				f = true;
				block++;
			}
			else if (f) {
				numberOfHoles += block;
				f = false;
				block = 0;
			}
		}
		if (!now)
			rowTransitions++;
	}

	int cnt = 0;
	int twoCnt = 0;
	for (int x = 0; x < 20; x++) {
		colTransitions += calc_01_change_count(rows[x]);
		uint16_t l = rows[x] << 1;
		l |= 0b100000000011;
		for (int y = 1; y < 10; y++) {
			uint16_t t = l >> y;
			if ((t & 0b111) == 0b101) {
				cnt++;
			}
			else {
				wellSums += well[cnt];
				cnt = 0;
				if ((t & 0b1111) == 0b1001) {
					twoCnt++;
				}
				else {
					twoWellSums += twoWell[twoCnt];
					twoCnt = 0;
				}
			}
		}
	}

	int f = 0;
	if (free0) {
		f = 800;
	}

	int sumV = f - 32 * rowTransitions
		- 93 * colTransitions
		//- 79 * numberOfHoles
		- 1000 * numberOfHoles
		- 34 * wellSums
		- 34 * twoWellSums;
	//如果左侧为空并且场上没有洞，惩罚消除1、2、3行，否则优先消除洞
	Value v((free0 && numberOfHoles == 0 ? clearScore[clean] : 0) - height * 22, sumV);
	return  Move(r, x, y, piece, v);
}


inline Move Board::backValue(int y, int r, Piece* piece) {

	PieceShape* shape = piece->pieceShapes[r];
	int x = minX(y, r, piece);
	// 超高
	if (x == 20) {
		return   Move(r, x, y, piece, Value(-99999, -99999));
	}
	//不允许放置
	if (x - shape->h + 1 < unIdentify) {
		return   Move(r, x, y, piece, Value(-99999, -99999));
	}

	bool isCreateHole = fill(r, x, y, piece);

	int clean = clearRows(x, shape->h);
	int height = 2 * x + shape->h - 1;
	int rowTransitions = 0;
	int colTransitions = 0;
	int numberOfHoles = 0;
	int wellSums = 0;
	int twoWellSums = 0;
	bool free0 = true;

	for (int y = 0; y < 10; y++) {
		bool now = true;
		bool f = false;
		int block = 0;

		for (int x = 19; x >= 0; x--) {
			bool isFill = isFilled(x, y);
			if (isFill != now) {
				now = isFill;
				rowTransitions++;
			}
			if (isFill) {

				if (y == 0) {
					free0 = false;
				}
				f = true;
				block++;
			}
			else if (f) {
				numberOfHoles += block;
				f = false;
				block = 0;
			}
		}
		if (!now)
			rowTransitions++;
	}

	int cnt = 0;
	int twoCnt = 0;
	for (int x = 0; x < 20; x++) {
		colTransitions += calc_01_change_count(rows[x]);
		uint16_t l = rows[x] << 1;
		l |= 0b100000000011;
		for (int y = 1; y < 10; y++) {
			uint16_t t = l >> y;
			if ((t & 0b111) == 0b101) {
				cnt++;
			}
			else {
				wellSums += well[cnt];
				cnt = 0;
				if ((t & 0b1111) == 0b1001) {
					twoCnt++;
				}
				else {
					twoWellSums += twoWell[twoCnt];
					twoCnt = 0;
				}
			}
		}
	}

	int f = 0;
	if (free0) {
		f = 800;
	}

	int sumV = f - 32 * rowTransitions
		- 93 * colTransitions
		//- 79 * numberOfHoles
		- 1000 * numberOfHoles
		- 34 * wellSums
		- 34 * twoWellSums;
	//如果左侧为空并且场上没有洞，惩罚消除1、2、3行，否则优先消除洞
	Value v((free0 && numberOfHoles == 0 ? clearScore[clean] : 0) - height * 22, sumV);
	return  Move(r, x, y, piece, v);
}

inline bool Board::isFilled(int x, int y) {
	if (x < 0)
		return true;
	return (rows[x] & (1 << y)) != 0;
}
inline bool Board::isFree(int x, int y) {
	if (x < 0)
		return false;
	return (rows[x] & (1 << y)) == 0;
}

void Board::paintAll(Move move) {
	Board copy = this->copy();
	list<Point> list;
	M m = move.m;
	PieceShape* shape = m.piece->pieceShapes[m.rotateIndex];
	Point* pp = shape->points;
	for (int i = 0; i < 4; i++) {
		Point point = *(pp + i);
		int x = copy.minX(m.y, m.rotateIndex, m.piece) - point.x;
		int y = m.y + point.y;
		list.push_back(Point(x, y));
	}
	string p[20];
	for (int x = 19; x >= 0; x--) {
		p[x] = "|";
		for (int y = 0; y < 10; y++) {
			if (copy.isFilled(x, y)) {
				p[x] += ("█");
			}
			else if (contain(list, x, y)) {
				p[x] += ("▓");
			}
			else {
				p[x] += ("  ");
			}
		}
		p[x] += "|";
	}

	while (move.next != NULL) {
		copy.fill(move);
		list.clear();
		m = move.next->m;
		shape = m.piece->pieceShapes[m.rotateIndex];
		pp = shape->points;
		for (int i = 0; i < 4; i++) {
			Point point = *(pp + i);
			int x = copy.minX(m.y, m.rotateIndex, m.piece) - point.x;
			int y = m.y + point.y;
			list.push_back(Point(x, y));
		}


		for (int x = 19; x >= 0; x--) {
			p[x] += "|";
			for (int y = 0; y < 10; y++) {
				if (copy.isFilled(x, y)) {
					p[x] += ("█");
				}
				else if (contain(list, x, y)) {
					p[x] += ("▓");
				}
				else {
					p[x] += ("  ");
				}
			}
			p[x] += "|";
		}
		move = *(move.next);
	}
	for (int x = 19; x >= 0; x--) {
		std::cout << p[x] << endl;
	}
	std::cout << "--------------------------------------------------------------------------" << endl;

}

inline bool Board::contain(list<Point> points, int x, int y) {
	for (Point p : points) {
		if (p.x == x && p.y == y) {
			return true;
		}
	}
	return false;
}

void Board::paint() {
	string s = string("\n");
	for (int x = 19; x >= 0; x--) {
		s.append("|");
		for (int y = 0; y < 10; y++) {
			if (isFilled(x, y)) {
				s.append("█");
			}
			else {
				s.append("  ");
			}
		}
		s.append("|\n");
	}
	s.append("----------------------\n"); s.append("nextList:");
	for (int i = 0; i < 6; i++) {
		s.append(next[i] == NULL ? "null" : string(1, next[i]->character()));
	}
	s.append("\n")
		.append("hold    : ").append(hold == NULL ? "null" : string(1, hold->character())).append("\n")
		.append("current : ").append(current == NULL ? "null" : string(1, current->character())).append("\n");
	std::cout << s << endl;

}

Move* Board::backGet() {
	if (hold == NULL) {
		hold = next[0];
		refreshNext();
		return new Move(0, hold->pieceShapes[0]->h, 0, hold, Value(0, 0), true);
	}

	set<Move> *moves = new set<Move>();
	backgetMoves(moves);
	if (moves->empty()) {
		return 0;
	}
	set<Move>::iterator itr;
	itr = moves->begin();
	Move b = *itr;
	Move *a = (Move*)&*itr;
	return  a;
}
Move* Board::get(int dep) {
	if (hold == NULL) {
		hold = next[0];
		refreshNext();
		return new Move(0, hold->pieceShapes[0]->h - 1, 9 - hold->pieceShapes[0]->w, hold, Value(0, 0), true);
	}

	set<Move> *moves = new set<Move>();
	getMoves(moves);
	if (moves->empty()) {
		return backGet();
	}
	set<Move>::iterator itr;
	itr = moves->begin();
	Move b = *itr;
	Move *a = (Move*)&*itr;
	if (dep <= 0) {
		return  a;
	}

	Move* rs = new Move();
	for (int i = 0; i < 3 && itr != moves->end(); i++) {
		Move *move = (Move*)&*itr;
		Board copy = this->copy();
		copy.useMove(*move);
		move->next = copy.get(dep - 1);
		int a = move->getAllMoveValue();
		int b = rs->getAllMoveValue();
		if (b == 0 || a > b) {
			rs = (Move*)&*itr;
		}
		itr++;
	}
	return rs;
}

inline void Board::getMoves(set<Move>* moves) {
	getPieceMoves(moves, current);
	getPieceMoves(moves, hold, true);
}
inline void Board::backgetMoves(set<Move>* moves) {
	backgetPieceMoves(moves, current, false);
	backgetPieceMoves(moves, hold, true);
}

inline void Board::backgetPieceMoves(set<Move> *moves, Piece* piece, bool isUseHold) {

	int index = piece->rEndIndex;
	for (int r = 0; r <= index; r++) {
		PieceShape* shape = piece->pieceShapes[r];
		int width = shape->w;
		for (int y = 0; y <= 10 - width; y++) {

			Board copy = this->copy();
			Move move = copy.backValue(y, r, piece);
			move.m.isUseHold = isUseHold;
			moves->insert(move);
		}
	}
}
inline void Board::getPieceMoves(set<Move> *moves, Piece* piece, bool isUseHold) {

	int index = piece->rEndIndex;
	for (int r = 0; r <= index; r++) {
		PieceShape* shape = piece->pieceShapes[r];
		int width = shape->w;
		for (int y = 0; y <= 10 - width; y++) {


			Board copy = this->copy();
			Move move = copy.value(y, r, piece);
			if (move.value.avgV == -99999) {
				continue;
			}
			move.m.isUseHold = isUseHold;
			moves->insert(move);
		}
	}
}

inline void Board::useMove(Move move) {
	if (move.m.isUseHold)
		hold = current;
	current = next[0];
	refreshNext();
	fill(move);
}

inline void Board::refreshNext() {
	memcpy(next, next + 1, sizeof(Piece*) * 5);
}

inline void Board::fill(Move move) {
	fill(move.m.rotateIndex, move.m.x, move.m.y, move.m.piece);
	clearRows(move.m.x, move.m.piece->pieceShapes[move.m.rotateIndex]->h);
}
