#include "food.h"
#include <Windows.h>

/*��궨λ*/
void gotoxy2(HANDLE hOut2, int x, int y)
{
	COORD pos;
	pos.X = x;  //������
	pos.Y = y;   //������
	SetConsoleCursorPosition(hOut2, pos);
}
//������ʾ���������
HANDLE hOut2 = GetStdHandle(STD_OUTPUT_HANDLE);

void Food::setFood()
{
	while (true)
	{
		foodx = rand() % (Wall::ROW - 2) + 1;
		foody = rand() % (Wall::COL - 2) + 1;

		//�����ߵ�λ������
		if (wall.getWall(foodx, foody) == ' ')
		{
			wall.setWall(foodx, foody, '#');
			gotoxy2(hOut2, foody * 2, foodx);
			cout << "#";
			break;
		}
	}


}

Food::Food(Wall& tempWall):wall(tempWall)
{
}
