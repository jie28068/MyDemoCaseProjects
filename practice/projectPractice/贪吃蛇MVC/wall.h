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

	//生成墙壁声明
	void initWall();

	//画墙声明
	void drawWali();

	//根据索引设置	二维数组的内容
	void setWall(int x, int y, char key);

	//根据索引获取当前符号内容
	char getWall(int x, int y);
private:
	char gameArray[ROW][COL];  //用二维数组做墙


};


#endif // !_WALL_HEAD
