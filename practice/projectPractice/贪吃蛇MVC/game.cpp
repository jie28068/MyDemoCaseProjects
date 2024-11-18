#if 1
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <ctime>
#include <conio.h>
#include <Windows.h>
using namespace std;
#include "wall.h"
#include "snake.h"
#include "food.h"
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */

/*光标定位*/
void gotoxy(HANDLE hOut, int x, int y)
{
	COORD pos;
	pos.X = x;  //横坐标
	pos.Y = y;   //纵坐标
	SetConsoleCursorPosition(hOut, pos);
}
//定义显示器句柄变量
HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

/*主程序*/
int main()
{
	//随机数种子
	srand((unsigned int)time(NULL));

	//死亡标识
	bool isdead = false;

	//记录上一次按键
	char perkey = NULL;

	Wall wall;
	wall.initWall();
	wall.drawWali();

	Food food(wall);
	food.setFood();

	Snake sanke(wall,food);
	sanke.initSnake();


	gotoxy(hOut, 0, Wall::ROW);
	cout << "得分：" << sanke.getscore() << endl;

	while (!isdead)
	{
		//接收输入
		char key = _getch();

		//不能让他一开始向左咬自己
		if (key == sanke.L && perkey==NULL)
		{
			continue;
		}

		//要输入正确的按键
		if (key == sanke.D || key == sanke.L || key == sanke.R || key == sanke.U)
		{
			//不许180度掉头走
			if ((key == sanke.D && perkey == sanke.U) ||
				(key == sanke.U && perkey == sanke.D) ||
				(key == sanke.R && perkey == sanke.L) ||
				(key == sanke.L && perkey == sanke.R)) 
			{
				key = perkey;
			}
			else
			{
				perkey = key;
			}
			//循环自己动
			do
			{
				perkey = key;
				if (sanke.move(key) == true)
				{
					//system("cls");
					//wall.drawWali();
					gotoxy(hOut, 0, Wall::ROW);
					
					cout << "得分：" << sanke.getscore() << endl;
					Sleep(sanke.sleep());
				}
				else
				{
					isdead = true;
					break;
				}
			} while (!_kbhit());//_kbhit()当没有键盘输入时，返回0
		}
		else
		{
			key = perkey;
		}

	}


	system("pause");
	return  EXIT_SUCCESS;
}
#endif;
