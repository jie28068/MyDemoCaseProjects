/*墙*/
#include "wall.h"

//生成墙
void Wall::initWall()
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			//放墙的位置,四条边
			if (i == 0 || j == 0 || i == ROW - 1 || j == COL - 1)
			{
				gameArray[i][j] = '*';
			}
			//其他位置为空格	
			else
			{
				gameArray[i][j] = ' ';
			}
		}
	}
}

//画墙
void Wall::drawWali()
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			cout << gameArray[i][j]<<' ';
		}
		if (i == 6)
		{
			cout << "作者：单杰";
		}
		cout << endl;
	}

}

//根据索引设置	二维数组的内容
void Wall::setWall(int x, int y, char key)
{
	gameArray[x][y] = key;
}

//根据索引获取当前符号内容
char Wall::getWall(int x, int y)
{
	return gameArray[x][y];
}
