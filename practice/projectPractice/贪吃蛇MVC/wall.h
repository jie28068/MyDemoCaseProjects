#pragma once
#ifndef _WALL_HEAD
#define _WALL_HEAD
#include <iostream>
#include <string>
using namespace std;
class Wall
{
public:
	enum 
	{
		ROW = 25,COL = 25
	};

	//����ǽ������
	void initWall();

	//��ǽ����
	void drawWali();

	//������������	��ά���������
	void setWall(int x, int y, char key);

	//����������ȡ��ǰ��������
	char getWall(int x, int y);
private:
	char gameArray[ROW][COL];  //�ö�ά������ǽ


};


#endif // !_WALL_HEAD
