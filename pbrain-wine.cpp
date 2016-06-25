#include<math.h>
#include<time.h>
#include<stdlib.h>
#include<conio.h>
#include<iostream>
#include<sstream>
#include<vector>
#include<string>
using namespace std;

const int win = 6;            // ����
const int flex4 = 5;          // ����
const int block4 = 4;         // ����
const int flex3 = 3;          // ����
const int block3 = 2;         // ����
const int flex2 = 1;          // ���
const int Ntype = 7;          // ���͸���
const int SearchDepth = 10;    // ����������
const int MaxSize = 20;          // �������ߴ�
inline int color(int step) {
	return (((step) % 2)?1 :2);
}

// ��������ṹ
struct xy {
	int x;
	int y;
};

// ��ѡ��ṹ
struct point {
	struct xy p;
	int val;
};

// ��������
int chessboard[MaxSize][MaxSize] = { 0 };
int size;
int step = 0;                   // ��ǰ����
int cpt;                        // ˫��������ɫ
struct xy chessxy[MaxSize*MaxSize];         // ������������
int branch = 10;                // ��֧��
double ThinkTime;               // ˼��ʱ��
int BestVal;                    // ����
int total;                      // �ڵ�
int ABcut;                      // ��֦
struct xy BestMove;             // ����߷�
int dx[4] = { 1, 0, 1, 1 };     //��������
int dy[4] = { 0, 1, 1, -1 };
int MeVal[5] = { 0, 10, 10, 35, 35 };
int YouVal[5] = { 0, 5, 5, 30, 30 };
int count[7] = { 0, 2, 3, 3, 4, 4, 5 };
int shape[MaxSize][MaxSize][2][4] = { 0 }; // ��������
int ChessRound[MaxSize][MaxSize] = { 0 }; //�����ŷ�
// ���ӻ���
class Chess {
public:
	void MakeMove(struct xy next);
	void DelMove();
	void Undo();
};

// ������
class Board :public Chess {
public:
	int CheckWin();
	int CheckXy(int x, int y);
};

// ���̷�����
class Analyse :public Board {
public:
	int ScoreMove(int x, int y);
	void ChessScore(int role, int *score, int *type);
	void UpdateType(int x, int y);
	void UpdateRound(int n);

private:
	int TypeLine(int role, int x, int y, int i, int j);
	void TypeCount(int x, int y, int role, int *type);

};

// ����AI��
class AI :public Analyse {
public:
	struct xy gobang();

private:
	int MaxDepth;
	struct point s[11][21];
	int evaluate2();
	int evaluate();
	int sort(struct point *a, int n);
	int GetMove(int depth);
	int AlphaBeta(int depth, int alpha, int beta);

};


//����
void Chess::MakeMove(struct xy next) {
	Analyse analyse;
	int x = next.x;
	int y = next.y;
	step++;
	chessboard[x][y] = color(step);
	chessxy[step].x = x;
	chessxy[step].y = y;
	analyse.UpdateRound(2);
	analyse.UpdateType(x, y);

}
//ɾ��
void Chess::DelMove() {
	Analyse analyse;
	int x = chessxy[step].x;
	int y = chessxy[step].y;
	chessboard[x][y] = 0;
	step--;
	analyse.UpdateType(x, y);
}
//����
void Chess::Undo() {
	if(step >= 2) {
		DelMove();
		DelMove();
	}
}
//�������Խ��
int Board::CheckXy(int x, int y) {
	if(x < 0 || x >= size || y < 0 || y >= size)
		return 0;
	else
		return 1;
}
//�ж����һ���Ƿ����
int Board::CheckWin() {
	int x = chessxy[step].x;
	int y = chessxy[step].y;
	int role = color(step);

	int a, b;
	int count;
	for(int i = 0; i<4; i++) {
		count = 1;
		a = x, b = y;
		for(int j = 0; j< 4; j++) {
			a += dx[i];
			b += dy[i];
			if(!CheckXy(a, b)) break;
			if(chessboard[a][b] != role) break;
			count++;
		}
		a = x, b = y;
		for(int k = 0; k< 4; k++) {
			a -= dx[i];
			b -= dy[i];
			if(!CheckXy(a, b)) break;
			if(chessboard[a][b] != role) break;
			count++;
		}
		if(count >= 5) return win;
	}

	return 0;
}
//����(x,y)����λ�õ�������Ϣ
void Analyse::UpdateType(int x, int y) {
	int a, b;
	for(int i = 0; i < 4; i++) {
		a = x;
		b = y;
		for(int j = 0; j < 4; j++) {
			a += dx[i];
			b += dy[i];
			if(!CheckXy(a, b))
				continue;
			if(chessboard[a][b] == 0 && ChessRound[a][b] == 0)
				continue;
			shape[a][b][0][i] = TypeLine(1, a, b, dx[i], dy[i]);
			shape[a][b][1][i] = TypeLine(2, a, b, dx[i], dy[i]);
		}
		a = x;
		b = y;
		for(int j = 0; j < 4; j++) {
			a -= dx[i];
			b -= dy[i];
			if(!CheckXy(a, b))
				continue;
			if(chessboard[a][b] == 0 && ChessRound[a][b] == 0)
				continue;
			shape[a][b][0][i] = TypeLine(1, a, b, dx[i], dy[i]);
			shape[a][b][1][i] = TypeLine(2, a, b, dx[i], dy[i]);
		}
	}
}
//���º����ŷ�
void Analyse::UpdateRound(int n) {
	int x, y;
	int i, j, k;
	for(i = 0; i < size; i++) {
		for(j = 0; j < size; j++) {
			ChessRound[i][j] = 0;
		}
	}
	for( k = 1; k <= step; k++) {
		x = chessxy[k].x;
		y = chessxy[k].y;
		for(i = x - n; i <= x + n; i++) {
			for(j = y - n; j <= y + n; j++) {
				if(!CheckXy(i, j))
					continue;
				ChessRound[i][j] = 1;
			}
		}
	}
}
//�жϽ�ɫrole�ڵ�(x,y)�ķ���(i,j)�ܳɵ�����
int Analyse::TypeLine(int role, int x, int y, int i, int j) {
	int a, b, k;
	int kong = 0, block = 0;
	int len = 1, len2 = 1, count = 1;
	a = x;
	b = y;
	for(k = 0; k < 4; k++) {
		a += i;
		b += j;
		if(!CheckXy(a, b)) {
			/* len2==kong+count��ʾ��һ�� �Ǽ������� */
			if(len2 == kong + count)
				block++;
			break;
		}
		if(chessboard[a][b] == role) {
			if(kong > 2 || kong + count > 4)
				break;
			count++;
			len++;
			len2 = kong + count;
		}
		else if(chessboard[a][b] == 0) {
			len++;
			kong++;
		}
		else {
			if(len2 == kong + count)
				block++;
			break;
		}
	}
	// �����м�ո�
	kong = len2 - count;
	a = x;
	b = y;
	for(k = 0; k < 4; k++) {
		a -= i;
		b -= j;
		if(!CheckXy(a, b)) {
			if(len2 == kong + count)
				block++;
			break;
		}
		if(chessboard[a][b] == role) {
			if(kong > 2 || kong + count > 4)
				break;
			count++;
			len++;
			len2 = kong + count;
		}
		else if(chessboard[a][b] == 0) {
			len++;
			kong++;
		}
		else {
			if(len2 == kong + count)
				block++;
			break;
		}
	}
	//�ж�����
	if(len >= 5 && count > 1) {
		if(count == 5)
			return win;
		if(len > 5 && len2 < 5 && block == 0) {
			switch(count) {
			case 2:
				return flex2;
			case 3:
				return flex3;
			case 4:
				return flex4;
			}
		}
		else {
			switch(count) {
			case 3:
				return block3;
			case 4:
				return block4;
			}
		}
	}
	return 0;
}
//ͳ�����θ���
void Analyse::TypeCount(int x, int y, int role, int *type) {
	int d[4];
	// �ĸ�����
	d[0] = shape[x][y][role - 1][0];
	d[1] = shape[x][y][role - 1][1];
	d[2] = shape[x][y][role - 1][2];
	d[3] = shape[x][y][role - 1][3];
	// ��¼����
	type[d[0]]++;
	type[d[1]]++;
	type[d[2]]++;
	type[d[3]]++;

}

//�����(x,y)�ķ���
int Analyse::ScoreMove(int x, int y) {
	int MeType[7] = { 0 }, YouType[7] = { 0 };
	int score = 0;
	int me = color(step + 1);
	
	TypeCount(x, y, me, MeType);
	TypeCount(x, y, 3-me, YouType);

	for(int i = 1; i < 5; i++) {
		score += MeVal[i] * MeType[i];
		score += YouVal[i] * YouType[i];
	}

	if(MeType[win] > 0)
		score += 10000;
	else if(YouType[win] > 0)
		score += 5000;
	else if(MeType[flex4] > 0 || MeType[block4] > 1)
		score += 2400;
	else if(MeType[block4] > 0 && MeType[flex3] > 0)
		score += 2000;
	else if(YouType[flex4] > 0 || YouType[block4] > 1)
		score += 1200;
	else if(YouType[block4] > 0 && YouType[flex3] > 0)
		score += 1000;
	else if(MeType[flex3] > 1)
		score += 400;
	else if(YouType[flex3] > 1)
		score += 200;

	return score;
}
//ͳ�ƽ�ɫrole���ܷ�
void Analyse::ChessScore(int role, int *score, int *type) {
	int x, y;
	int val[5] = { 0, 2, 2, 5, 5 };
	*score = 0;

	int i;
	for(i = role; i <= step; i += 2) {
		x = chessxy[i].x;
		y = chessxy[i].y;
		TypeCount(x, y, role, type);
	}
	int c;

	for(i = 1; i < Ntype; i++) {
		c = type[i] % count[i];
		type[i] = type[i] / count[i];
		if(c == count[i] - 1)
			type[i]++;
	}

	// ���ܻ�������������������ķ�ֵ
	for(i = 1; i < 5; i++) {
		*score += val[i] * type[i];
	}
}
// ����˫��������������
int AI::evaluate2() {
	int Cscore, Hscore;
	int Ctype[7] = { 0 }, Htype[7] = { 0 };
	int me = color(step);
	
	ChessScore(me, &Cscore, Ctype);
	ChessScore(3 - me, &Hscore, Htype);
	
	if(Ctype[win] > 0)
		return 10000;
	else if(Htype[flex4] > 0 || Htype[block4] > 0)
		return -10000;
	else if(Ctype[flex4] > 0 || Ctype[block4] > 1)
		return 10000;
	else if(Htype[flex3] > 0 && Ctype[block4]==0)
		return -10000;

	return Cscore - Hscore;
}
//��������
int AI::evaluate() {
	int val, i, j;
	int max = 0;
	struct xy p;
	// ѡ����ѵ�
	for(i = 0; i < size; i++) {
		for(j = 0; j < size; j++) {
			if(chessboard[i][j] == 0) {
				if(ChessRound[i][j]) {
					val = ScoreMove(i, j);
					if(val > max) {
						max = val;
						p.x = i;
						p.y = j;
					}
				}
			}
		}
	}
	// ������ѵ�����֮��ľ���
	MakeMove(p);
	val = evaluate2();
	DelMove();
	return val;
}

// ����
int AI::sort(struct point *a, int n) {
	int i, j;
	struct point key;
	for(i = 2; i <= n; i++) {
		key = a[i];
		for(j = i; j > 1 && a[j - 1].val < key.val; j--) {
			a[j] = a[j - 1];
		}
		a[j] = key;
	}
	return 0;
}

// ������һ�������߷��������ܸ���
int AI::GetMove(int depth) {
	int i, j;
	int n = 0;
	int count;
	struct point sp[MaxSize*MaxSize];
	for(i = 0; i < size; i++) {
		for(j = 0; j < size; j++) {
			if(chessboard[i][j] == 0) {
				if(ChessRound[i][j]) {
					n++;
					sp[n].p.x = i;
					sp[n].p.y = j;
					sp[n].val = ScoreMove(i, j);
				}
			}
		}
	}
	// �յ�����
	sort(sp, n);

	// ���Ʒ�֧��
	if(n < branch)
		count = n;
	else
		count = branch;

	// ���ƿ�ѡ��
	for(i = 1; i <= count; i++) {
		s[depth][i] = sp[i];
	}

	return count;
}

int AI::AlphaBeta(int depth, int alpha, int beta) {
	int val;
	total++;
	// �Է����һ������
	if(CheckWin())
		return -10000;
	// Ҷ�ڵ�
	if(depth == 0)
		return evaluate();
	int count = GetMove(depth);
	// ���������߷�
	for(int i = 1; i <= count; i++) {
		MakeMove(s[depth][i].p);
		val = -AlphaBeta(depth - 1, -beta, -alpha);
		DelMove();
		if(val >= beta) {
			if(depth == MaxDepth)
				BestMove = s[depth][i].p;
			ABcut++;
			return val;
		}
		if(val > alpha) {
			alpha = val;
			if(depth == MaxDepth)
				BestMove = s[depth][i].p;
		}
	}
	return alpha;
}
//��ȡ��ѵ�
struct xy AI::gobang() {
	clock_t start, finish;
	start = clock();
	//cout << "����˼����......\n";
	total = 0;
	ABcut = 0;
	srand(time(NULL));
	// ��һ�������ĵ�
	if(step == 0) {
		BestMove.x = size / 2;
		BestMove.y = size / 2;
		return BestMove;
	}
	// �ڶ����������
	if(step == 2 || step == 1) {
		int rx, ry;
		int d = step * 2 + 1;
		do {
			rx = rand() % d + chessxy[1].x - step;
			ry = rand() % d + chessxy[1].y - step;
		} while(chessboard[rx][ry] != 0);
		BestMove.x = rx;
		BestMove.y = ry;
		return BestMove;
	}
	// ������������
	for(int i = 2; i <= SearchDepth; i += 2) {
		MaxDepth = i;
		BestVal = AlphaBeta(i, -10001, 10000);
		if(BestVal == 10000)
			break;
	}
	finish = clock();
	ThinkTime = (double)(finish - start) / CLOCKS_PER_SEC;
	return BestMove;
}

void reset() {
	int remStep=step;
	Chess chess;
	for(int i = 1; i <= remStep; i++) {
		chess.DelMove();
	}

}
int gomocup() {
	string command;
	AI ai;
	xy input,best;
	char dot;
	vector<int> cl;
	vector<xy> ps;
	bool last_is_board = false;
	while(1) {

		if(last_is_board && cl.back() == 2) {
			command = "BEGIN";
		}
		else {
			cin >> command;
			for(int i=0;i<command.size();i++) {
				char c=command[i];
				if(c >= 'a' && c <= 'z') {
					command[i] += 'A' - 'a';
				}
			}
		}
		if(last_is_board) {
			for(int i = 0; i < ps.size(); i++) {
				ai.MakeMove(ps[i]);
			}
			ps.clear();
			cl.clear();
			last_is_board = false;
		}
		if(command == "START") {
			cin >> size;
			if(size > 30 || size <= 5) {
				cout << "ERROR" << endl;
			}
			else {
				//SIZE = size;
				cout << "OK" << endl;
			}
		}
		if(command == "RESTART") {
			reset();
			cout << "OK" << endl;
			//command = "BEGIN";
		}
		else if(command == "TAKEBACK") {
			ai.DelMove();
			//TODO
		}
		else if(command == "BEGIN") {
			best = ai.gobang();
			ai.MakeMove(best);
			std::cout << (int)best.x << "," << (int)best.y << endl;
		}
		else if(command == "TURN") {
			cin >> input.x >> dot >> input.y;
			if(input.x < 0 || input.x >= size || input.y < 0 || input.y >= size) {
				cout << "ERROR" << endl;
			}
			else {
				ai.MakeMove(input);
				best = ai.gobang();
				ai.MakeMove(best);
				cout << (int)best.x << "," << (int)best.y << endl;
			}
		}

		else if(command == "BOARD") {
			cl.clear();
			ps.clear();
			reset();
			last_is_board = true;
			stringstream ss;
			cin >> command;
			while(command != "DONE") {
				ss.clear();
				ss << command;
				xy m;
				int  c;
				ss >> m.x >> dot >> m.y >> dot >> c;
				if(m.x < 0 || m.x >= size || m.y < 0 || m.y >= size || (c != 1 && c != 2)) {
					cout << "ERROR" << endl;
				}
				else {
					ps.push_back(m);
					cl.push_back(c);
				}
				cin >> command;
			}

		}
		else if(command == "INFO") {
			int SEARCH_TIME;
			int timeout_match;
			int time_left;
			string key;
			cin >> key;
			if(key == "timeout_turn") {//ֻ��������
				int value;
				cin >> value;
				if(value >= 100) {
					SEARCH_TIME = value - 170;
				}
				else {
					SEARCH_TIME = 100;
				}

			}
			else if(key == "timeout_match") {
				cin >> timeout_match;
			}
			else if(key == "max_memory") {
				int value;
				cin >> value;
				//TODO
			}
			else if(key == "time_left") {

				cin >> time_left;
			}
			else if(key == "game_type") {
				int value;
				cin >> value;
				//TODO
			}
			else if(key == "rule") {
				int value;
				cin >> value;
				//TODO
			}
			else if(key == "folder") {
				string t;
				cin >> t;
			}
		}
		else if(command == "END") {

		}

	}

}

int main() {
	gomocup();
	return 0;
}

