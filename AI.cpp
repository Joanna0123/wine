#include<math.h>
#include<time.h>
#include<stdlib.h>
#include<conio.h>
#include<iostream>
using namespace std;

#define win    6                //����
#define flex4  5                // ����
#define block4  4               // ����
#define flex3      3            // ����
#define block3   2              // ����
#define flex2    1              // ���
#define Ntype    7              // ���͸���
#define SearchDepth  8          // ����������
#define size 15                 // ���̴�С
#define color(step)       (((step)-1)%2+1)

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
int chessboard[size][size] = { 0 };
int step = 0;                   // ��ǰ����
int cpt;                        // ˫��������ɫ
int mode;                       // ��Ϸģʽ
struct xy chessxy[226];         // ������������
int branch = 10;                // ��֧��
double ThinkTime;               // ˼��ʱ��
int BestVal;                    // ����
int total;                      // �ڵ�
int ABcut;                      // ��֦
struct xy BestMove;             // ����߷�
int dx[4] = { 1, 0, 1, 1 };     //��������
int dy[4] = { 0, 1, 1, -1 };
int shape[size][size][2][4] = { 0 }; // ��������
int ChessRound[size][size] = { 0 }; //�����ŷ�
// ���ӻ���
class Chess {
public:
  void MakeMove(struct xy next);
  void DelMove();
  void Undo();

};

// ������
class Board:public Chess {
public:
  int CheckWin();
  int CheckXy(int x, int y);
};

// ���̷�����
class Analyse:public Board {
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
class AI:public Analyse {
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

// ������
class Gui:public Board {
public:
  void ShowThink();
  void DrawChess();
  void GetXy(int *x, int *y);

private:
  int GoToP(int *x, int *y);
};

// ��Ϸ������
class Game:public Board {
public:
  void Play();
  void ChessStart();

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
  if (step >= 2) {
    DelMove();
    DelMove();
  }
}
//�������Խ��
int Board::CheckXy(int x, int y) {
  if (x < 0 || x >= size || y < 0 || y >= size)
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
	for(int i=0;i<4;i++){
		count = 1;
		a = x,b = y;
		for (int j= 0; j< 4; j++) {
			a += dx[i];
			b += dy[i];
			if (!CheckXy(a, b)) break;
			if (chessboard[a][b] != role) break;
			count++;
		}
		a = x,b = y;
		for (int k= 0; k< 4; k++) {
			a -= dx[i];
			b -= dy[i];
			if (!CheckXy(a, b)) break;
			if (chessboard[a][b] != role) break;
			count++;
		}
		if (count >= 5) return win;
  }
  
  return 0;
}
//����(x,y)����λ�õ�������Ϣ
void Analyse::UpdateType(int x, int y) {
  int a, b;
  for (int i = 0; i < 4; i++) {
    a = x;
    b = y;
    for (int j = 0; j < 4; j++) {
      a += dx[i];
      b += dy[i];
      if (!CheckXy(a, b))
        continue;
      if (chessboard[a][b] == 0 && ChessRound[a][b] == 0)
        continue;
      shape[a][b][0][i]
        = TypeLine(1, a, b, dx[i], dy[i]);
      shape[a][b][1][i]
        = TypeLine(2, a, b, dx[i], dy[i]);
    }
    a = x;
    b = y;
    for (int j = 0; j < 4; j++) {
      a -= dx[i];
      b -= dy[i];
      if (!CheckXy(a, b))
        continue;
      if (chessboard[a][b] == 0 && ChessRound[a][b] == 0)
        continue;
      shape[a][b][0][i]
        = TypeLine(1, a, b, dx[i], dy[i]);
      shape[a][b][1][i]
        = TypeLine(2, a, b, dx[i], dy[i]);
    }
  }
}
//���º����ŷ�
void Analyse::UpdateRound(int n) {
  int x, y;
  int i, j;
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      ChessRound[i][j] = 0;
    }
  }
  for (int k = 1; k <= step; k++) {
    x = chessxy[k].x;
    y = chessxy[k].y;
    for (i = x - n; i <= x + n; i++) {
      for (j = y - n; j <= y + n; j++) {
        if(!CheckXy(i,j))
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
  for (k = 0; k < 4; k++) {
    a += i;
    b += j;
    if (!CheckXy(a, b)) {
      /* len2==kong+count��ʾ��һ�� �Ǽ������� */
      if (len2 == kong + count)
        block++;
      break;
    }
    if (chessboard[a][b] == role) {
      if (kong > 2 || kong + count > 4)
        break;
      count++;
      len++;
      len2 = kong + count;
    } else if (chessboard[a][b] == 0) {
      len++;
      kong++;
    } else {
      if (len2 == kong + count)
        block++;
      break;
    }
  }
  // �����м�ո�
  kong = len2 - count;
  a = x;
  b = y;
  for (k = 0; k < 4; k++) {
    a -= i;
    b -= j;
    if (!CheckXy(a, b)) {
      if (len2 == kong + count)
        block++;
      break;
    }
    if (chessboard[a][b] == role) {
      if (kong > 2 || kong + count > 4)
        break;
      count++;
      len++;
      len2 = kong + count;
    } else if (chessboard[a][b] == 0) {
      len++;
      kong++;
    } else {
      if (len2 == kong + count)
        block++;
      break;
    }
  }
  //�ж�����
  if (len >= 5 && count > 1) {
    if (count == 5)
      return win;
    if (len > 5 && len2 < 5 && block == 0) {
      switch (count) {
      case 2:
        return flex2;
      case 3:
        return flex3;
      case 4:
        return flex4;
      }
    } else {
      switch (count) {
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
  int MeType[7] = { 0 }, YouType[7] = {
  0};
  int score = 0;

  int me = color(step + 1);
  int you = 3 - me;

  int MeVal[5] = { 0, 10, 10, 35, 35 };
  int YouVal[5] = { 0, 5, 5, 30, 30 };

  TypeCount(x, y, me, MeType);
  TypeCount(x, y, you, YouType);

  for (int i = 1; i < 5; i++) {
    score += MeVal[i] * MeType[i];
    score += YouVal[i] * YouType[i];
  }

  if (MeType[win] > 0)
    score += 10000;
  else if (YouType[win] > 0)
    score += 5000;
  else if (MeType[flex4] > 0 || MeType[block4] > 1)
    score += 2400;
  else if (MeType[block4] > 0 && MeType[flex3] > 0)
    score += 2000;
  else if (YouType[flex4] > 0 || YouType[block4] > 1)
    score += 1200;
  else if (YouType[block4] > 0 && YouType[flex3] > 0)
    score += 1000;
  else if (MeType[flex3] > 1)
    score += 400;
  else if (YouType[flex3] > 1)
    score += 200;

  return score;
}
//ͳ�ƽ�ɫrole���ܷ�
void Analyse::ChessScore(int role, int *score, int *type) {
  int i;
  int x, y;
  int count[7] = { 0, 2, 3, 3, 4, 4, 5 };
  int val[5] = { 0, 2, 2, 5, 5 };
  *score = 0;

  for (i = role; i <= step; i += 2) {
    x = chessxy[i].x;
    y = chessxy[i].y;
    TypeCount(x, y, role, type);
  }

  for (i = 1; i < Ntype; i++) {
    int c = type[i] % count[i];
    type[i] = type[i] / count[i];
    if (c == count[i] - 1)
      type[i]++;
  }

  // ���ܻ�������������������ķ�ֵ
  for (i = 1; i < 5; i++) {
    *score += val[i] * type[i];
  }
}
// ����˫��������������
int AI::evaluate2() {
  int Cscore, Hscore;
  int Ctype[7] = { 0 }, Htype[7] = {
  0};
  int me = color(step);
  ChessScore(me, &Cscore, Ctype);
  ChessScore(3 - me, &Hscore, Htype);
  if (Ctype[win] > 0)
    return 10000;
  else if (Htype[flex4] > 0 || Htype[block4] > 0)
    return -10000;
  else if (Ctype[flex4] > 0 || Ctype[block4] > 1)
    Cscore += 250;
  else if (Ctype[block4] > 0 && Ctype[flex3] > 0)
    Cscore += 200;
  else if (Htype[flex3] > 0)
    Hscore += 150;
  else if (Ctype[flex3] > 1)
    Cscore += 80;

  return Cscore - Hscore;
}
//��������
int AI::evaluate() {
  int val, i, j;
  int max = 0;
  struct xy p;
  // ѡ����ѵ�
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      if (chessboard[i][j] == 0) {
        if (ChessRound[i][j]) {
          val = ScoreMove(i, j);
          if (val > max) {
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
  for (i = 2; i <= n; i++) {
    key = a[i];
    for (j = i; j > 1 && a[j - 1].val < key.val; j--) {
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
  struct point sp[225];
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      if (chessboard[i][j] == 0) {
        if (ChessRound[i][j]) {
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
  if (n < branch)
    count = n;
  else
    count = branch;

  // ���ƿ�ѡ��
  for (i = 1; i <= count; i++) {
    s[depth][i] = sp[i];
  }

  return count;
}

int AI::AlphaBeta(int depth, int alpha, int beta) {
  int val;
  total++;
  // �Է����һ������
  if (CheckWin())
    return -10000;
  // Ҷ�ڵ�
  if (depth == 0)
    return evaluate();
  int count = GetMove(depth);
  // ���������߷�
  for (int i = 1; i <= count; i++) {
    MakeMove(s[depth][i].p);
    val = -AlphaBeta(depth - 1, -beta, -alpha);
    DelMove();
    if (val >= beta) {
      if (depth == MaxDepth)
        BestMove = s[depth][i].p;
      ABcut++;
      return val;
    }
    if (val > alpha) {
      alpha = val;
      if (depth == MaxDepth)
        BestMove = s[depth][i].p;
    }
  }
  return alpha;
}
//��ȡ��ѵ�
struct xy AI::gobang() {
  int i;
  clock_t start, finish;
  start = clock();
  cout << "����˼����......\n";
  total = 0;
  ABcut = 0;
  srand(time(NULL));
  // ��һ�������ĵ�
  if (step == 0) {
    BestMove.x = size / 2;
    BestMove.y = size / 2;
    return BestMove;
  }
  // �ڶ����������
  if (step == 2 || step == 1) {
    int rx, ry;
    int d=step*2+1;
    do {
      rx = rand() % d + chessxy[1].x - step;
      ry = rand() % d + chessxy[1].y - step;
    } while (chessboard[rx][ry] != 0);
    BestMove.x = rx;
    BestMove.y = ry;
    return BestMove;
  }
  // ������������
  for (i = 2; i <= SearchDepth; i += 2) {
    MaxDepth = i;
    BestVal = AlphaBeta(i, -10001, 10000);
    if (BestVal == 10000)
      break;
  }
  finish = clock();
  ThinkTime = (double)(finish - start) / CLOCKS_PER_SEC;
  return BestMove;
}
//��ʾ˼����Ϣ
void Gui::ShowThink() {
  char x = BestMove.x + 65;
  int y = 15-BestMove.y;
  /* cout<<"-------------------------------\n"; cout<<" ����:jinjiebang \n";
     cout<<" ����:724895582@qq.com \n"; cout
     <<"-------------------------------\n"; */
  cout << " ˼��ʱ��: " << ThinkTime<<"s";
  cout << " �Ѷ�: " << branch << endl;
  cout << " ��������: " << total;
  cout << " ��֦: " << ABcut;
  cout << endl;
  cout << " ��ѵ�:" << x << y;
  cout << " ����: " << step;
  cout << " ����: " << BestVal;
  cout << endl;
}
//�����û�ѡ������,��������ǰ����
int Gui::GoToP(int *x, int *y) {
  struct xy p;
  AI ai;
  int dx = *x, dy = *y;
  int c = getch();
  switch (c) {
  case 'w':
    dy--;
    break;
  case 's':
    dy++;
    break;
  case 'a':
    dx--;
    break;
  case 'd':
    dx++;
    break;
  case 'u':
    Undo();
    break;
  case 'p':
    mode = 4;
  case 'q':
    if (mode == 3)
      mode = cpt = color(step + 1);
    p = ai.gobang();
    *x = p.x;
    *y = p.y;
    return 0;
  case 'y':
    return 0;
  default:
    return 1;
  }
  if (!CheckXy(dx, dy))
    return 1;
  int temp = chessboard[dx][dy];
  chessboard[dx][dy] = 3;
  DrawChess();
  chessboard[dx][dy] = temp;
  *x = dx;
  *y = dy;
  return 1;
}
//������
void Gui::DrawChess() {
  system("cls");
  ShowThink();
  int i, j;
  for (i = 0; i < size; i++) {
    if (15-i< 10)
      cout << " " << 15-i;
    else
      cout << 15-i;
    for (j = 0; j < size; j++) {
      switch (chessboard[j][i]) {
      case 0:
        cout << "+ ";
        break;
      case 1:
        cout << "O ";
        break;
      case 2:
        cout << "X ";
        break;
      case 3:
        cout << "? ";
        break;
      }
    }
    cout << endl;
  }
  cout << "  ";
  for (i = 0; i < size; i++) {
    char c=65+i;
    cout << c << " ";
  }
  cout << endl;
}
//��ȡ�û�ѡ������
void Gui::GetXy(int *x, int *y) {
  int i, j;
  int flag;
  int dx, dy;
  if (step != 0) {
    dx = chessxy[step].x;
    dy = chessxy[step].y;
    for (i = dx - 2; i <= dx + 2; i++) {
      for (j = dy - 2; j <= dy + 2; j++) {
        if (!CheckXy(i, j))
          continue;
        if (chessboard[i][j] == 0) {
          dx = i;
          dy = j;
          break;
        }
      }
      if (chessboard[dx][dy] == 0)
        break;
    }
  } else {
    dx = dy = size / 2;
  }
  chessboard[dx][dy] = 3;
  DrawChess();
  chessboard[dx][dy] = 0;
  *x = dx;
  *y = dy;
  cout << "��wasd�ƶ���yȷ��\n";
  cout << "u: ����";
  cout << "  q: ���Լ���";
  cout << "  p: �����Զ�����\n";
  do {
    flag = (GoToP(x, y));
  } while (flag || chessboard[*x][*y] != 0);
}
//������Ϸ����
void Game::Play() {
  struct xy next;
  Gui gui;
  AI ai;
  gui.DrawChess();              // ������
  ChessStart();                 // ��ʼ�����
  while (1) {
    if (mode == 4) {
      next = ai.gobang();
    } else if (mode == 3) {
      gui.GetXy(&next.x, &next.y);
    } else {
      if (color(step) == cpt) {
        gui.GetXy(&next.x, &next.y);
      } else {
        next = ai.gobang();
      }
    }
    MakeMove(next);
    gui.DrawChess();
    // ����ʤ��
    if (CheckWin()) {
      if (color(step) == cpt)
        cout << "����Ӯ��\n";
      else
        cout << "��Ӯ��^_^\n";
      ChessStart();
    }
  }
}
//��Ϸ���¿�ʼ
void Game::ChessStart() {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      chessboard[i][j] = 0;
    }
  }
  step = 0;
  BestVal = 0;
  cout << "������";
  cout << " ��������:1";
  cout << " ���Ժ���:2";
  cout << " ����:3\n";
  cin >> mode;
  cpt = mode;
  cout << "�Ѷȵȼ�\n";
  cout << "����:6 �м�:10 �߼�:14\n";
  cin >> branch;
}


main() {
  Game game;
  game.Play();                  // ��Ϸ����

  return 0;
}
