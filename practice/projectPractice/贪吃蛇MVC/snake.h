#pragma once
#ifndef _SNAKE_HEAD
#define _SNAKE_HEAD
#include <iostream>
using namespace std;
#include "wall.h"
#include "food.h"

class Snake
{
public:
	//构造函数
	Snake(Wall &tempWall, Food &tempFood);

	//枚举
	enum{U='w',D='s',L='a',R='d' };

	//定义链表
	struct Point
	{
		//数据域
		int x;
		int y;

		//指针域
		Point* next;
	};
	//初始化节点
	void initSnake();

	//销毁节点
	void deleteSnake();

	//添加节点
	void addPoint(int x, int y);

	//删除节点
	void delPoint();

	//移动
	bool move(char k);

	//头结点
	Point* head;

	//维护一个墙
	Wall& wall;

	//维护一个食物
	Food& food;

	//尾巴标识
	bool isend;

	//获取蛇长度
	int count();

	//获取刷屏时间
	int sleep();

	//设定难度
	//获取分数
	int getscore();
};

#endif // !_SNAKE_HEAD