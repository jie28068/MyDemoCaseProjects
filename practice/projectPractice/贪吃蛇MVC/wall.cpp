/*ǽ*/
#include "wall.h"

//����ǽ
void Wall::initWall()
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			//��ǽ��λ��,������
			if (i == 0 || j == 0 || i == ROW - 1 || j == COL - 1)
			{
				gameArray[i][j] = '*';
			}
			//����λ��Ϊ�ո�	
			else
			{
				gameArray[i][j] = ' ';
			}
		}
	}
}

//��ǽ
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
			cout << "���ߣ�����";
		}
		cout << endl;
	}

}

//������������	��ά���������
void Wall::setWall(int x, int y, char key)
{
	gameArray[x][y] = key;
}

//����������ȡ��ǰ��������
char Wall::getWall(int x, int y)
{
	return gameArray[x][y];
}
