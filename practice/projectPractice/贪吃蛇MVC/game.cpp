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

/*��궨λ*/
void gotoxy(HANDLE hOut, int x, int y)
{
	COORD pos;
	pos.X = x;  //������
	pos.Y = y;   //������
	SetConsoleCursorPosition(hOut, pos);
}
//������ʾ���������
HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

/*������*/
int main()
{
	//���������
	srand((unsigned int)time(NULL));

	//������ʶ
	bool isdead = false;

	//��¼��һ�ΰ���
	char perkey = NULL;

	Wall wall;
	wall.initWall();
	wall.drawWali();

	Food food(wall);
	food.setFood();

	Snake sanke(wall,food);
	sanke.initSnake();


	gotoxy(hOut, 0, Wall::ROW);
	cout << "�÷֣�" << sanke.getscore() << endl;

	while (!isdead)
	{
		//��������
		char key = _getch();

		//��������һ��ʼ����ҧ�Լ�
		if (key == sanke.L && perkey==NULL)
		{
			continue;
		}

		//Ҫ������ȷ�İ���
		if (key == sanke.D || key == sanke.L || key == sanke.R || key == sanke.U)
		{
			//����180�ȵ�ͷ��
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
			//ѭ���Լ���
			do
			{
				perkey = key;
				if (sanke.move(key) == true)
				{
					//system("cls");
					//wall.drawWali();
					gotoxy(hOut, 0, Wall::ROW);
					
					cout << "�÷֣�" << sanke.getscore() << endl;
					Sleep(sanke.sleep());
				}
				else
				{
					isdead = true;
					break;
				}
			} while (!_kbhit());//_kbhit()��û�м�������ʱ������0
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
