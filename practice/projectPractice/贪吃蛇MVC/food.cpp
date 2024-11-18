#include "food.h"
#include <Windows.h>

/*光标定位*/
void gotoxy2(HANDLE hOut2, int x, int y)
{
	COORD pos;
	pos.X = x;  //横坐标
	pos.Y = y;   //纵坐标
	SetConsoleCursorPosition(hOut2, pos);
}
//定义显示器句柄变量
HANDLE hOut2 = GetStdHandle(STD_OUTPUT_HANDLE);

void Food::setFood()
{
	while (true)
	{
		foodx = rand() % (Wall::ROW - 2) + 1;
		foody = rand() % (Wall::COL - 2) + 1;

		//不是蛇的位置生成
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
