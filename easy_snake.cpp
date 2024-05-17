/**
 * cpp file encoding: utf-8
 *
 * 地图分为有边界和无边界
 * 有边界：蛇头撞墙会死
 * 无边界：蛇头穿墙会传送到地图的另一边
 *
 * 操作：WASD变向，按住J加速
 * 游戏结束后：按R再来一把，按E退出
 *
 */
#include <iostream>
#include <conio.h>
#include <ctime>
#include <cstdlib>
#include <deque>
#include <cstring>
#include <windows.h>
#include <unistd.h>
#include <cassert>
using namespace std;

/// @brief 用来移动光标
void cursor(int x, int y)
{
	COORD pos;
	HANDLE han = GetStdHandle(STD_OUTPUT_HANDLE);
	pos.X = y, pos.Y = x;
	SetConsoleCursorPosition(han, pos);
}

/// @brief 蛇节点，也用来表示坐标
struct SnakeNode
{
	int x, y;
	SnakeNode operator+(const SnakeNode &s) const
	{
		return {s.x + x, s.y + y};
	}
	inline bool operator==(const SnakeNode &s) const
	{
		return x == s.x && y == s.y;
	}
};

/// @brief 方向枚举
enum Direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

/// @brief 地图行数
int row;
/// @brief 地图列数
int col;
/// @brief 地图是否是无边界的
bool borderless;

class Snake
{
private:
	/// @brief 双向队列存蛇身，其中第一个元素是蛇头
	deque<SnakeNode> body;
	/// @brief 食物的位置
	SnakeNode food;

	void writeNode(const SnakeNode &node, char ch)
	{
		cursor(node.x + 1, node.y + 1);
		cout << ch;
	}

public:
	/// @brief 蛇头前进方向
	enum Direction direction;
	int length = 1;
	bool alive()
	{
		SnakeNode head = body.front();

		if (head.x < 0 || head.x >= row || head.y < 0 || head.y >= col)
			return false;

		for (auto iter = body.begin() + 1; iter != body.end(); ++iter)
		{
			if (*iter == head)
			{
				return false;
			}
		}
		return true;
	}

	Snake()
	{
		direction = Direction::RIGHT;
		SnakeNode head = {row / 2, col / 2};
		body.push_back(head);
		writeNode(head, 'H');
		food = {rand() % row, rand() % col};
		writeNode(food, '*');
	}

	// 蛇移动
	void move()
	{
		static const SnakeNode posDelta[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

		// tmp存蛇头的下一个位置
		SnakeNode tmp = body.front() + posDelta[direction];

		// 如果地图无边界，把蛇传送到另一边
		if (borderless)
		{
			if (tmp.x >= row)
				tmp.x = 0;
			else if (tmp.x < 0)
				tmp.x = row - 1;
			if (tmp.y >= col)
				tmp.y = 0;
			else if (tmp.y < 0)
				tmp.y = col - 1;
		}

		// 原蛇头蛇尾
		SnakeNode tail = body.back();
		SnakeNode head = body.front();

		// 把屁股删了，头接上一个
		body.pop_back();
		body.push_front(tmp);

		// 地图上改几个字符就行了
		writeNode(head, 'O');
		writeNode(tail, ' ');
		writeNode(tmp, 'H');
		writeNode(food, '*');

		// 如果吃到食物，给屁股加一个点
		if (tmp == food)
		{
			body.push_back(tail);
			writeNode(food, 'X');
			food = {rand() % row, rand() % col};
			++length;
			cursor(row / 2 - 2, col + 18);
			cout << length;
			writeNode(food, '*');
		}

		// 把光标移走
		cursor(0, 0);
	}
};

/// @brief 开一局游戏
void play()
{
	system("cls");
	// 输入大小，是否有边界
	cout << "Input the row (20 recommended):" << endl;
	cin >> row;
	cout << "Input the column (30 recommended):" << endl;
	cin >> col;
	assert(row > 0 && col > 0);
	cout << "Is the map borderless? (y/n):" << endl;
	char ch;
	cin >> ch;
	ch = tolower(ch);
	borderless = ch == 'y';
	ch = borderless ? '.' : '#';
	system("cls");
	// 打印墙壁，有边界打印'#'，无边界打印'.'
	for (int i = 0; i < col + 2; ++i)
	{
		cout << ch;
	}
	for (int i = 1; i <= row; ++i)
	{
		cursor(i, 0);
		cout << ch;
		cursor(i, col + 1);
		cout << ch;
	}
	cursor(row + 1, 0);
	for (int i = 0; i < col + 2; ++i)
	{
		cout << ch;
	}

	// 打印分数
	cursor(row / 2 - 2, col + 10);
	cout << "Length: 1";
	cursor(row / 2, col + 10);
	cout << "Press \"J\" to boost";
	cursor(row / 2 + 2, col + 10);
	cout << "Press \"WASD\" to change direction";
	cursor(row / 2 + 4, col + 10);
	cout << "Boardless: " << (borderless ? "true" : "false");

	// 初始化蛇
	Snake snake;

	// 循环
	while (snake.alive())
	{
		char key = 0;
		while (kbhit())
			key = getch();
		key = tolower(key);
		// 蛇不能直接回头
		switch (key)
		{
		case 'w':
			if (snake.direction != Direction::DOWN)
				snake.direction = Direction::UP;
			break;
		case 'a':
			if (snake.direction != Direction::RIGHT)
				snake.direction = Direction::LEFT;
			break;
		case 's':
			if (snake.direction != Direction::UP)
				snake.direction = Direction::DOWN;
			break;
		case 'd':
			if (snake.direction != Direction::LEFT)
				snake.direction = Direction::RIGHT;
			break;
		}
		snake.move();
		// 按j加速
		if (key == 'j')
			Sleep(100);
		else
			Sleep(250);
	}
	// 寄
	cursor(row + 2, 0);
	cout << "Game Over" << endl;
	cout << "Your Length is: " << snake.length << endl;
}

int main()
{
	// 随机数种子
	srand(time(0));

	char ch;
	do
	{
		// 开一局
		play();
		// 判断重开和退出
		cout << "Press \"R\" to restart or \"E\" to exit.";
		do
		{
			ch = getch();
			ch = tolower(ch);
		} while (ch != 'r' && ch != 'e'); // 让ch只能是 r e
	} while (ch != 'e');
	return 0;
}
