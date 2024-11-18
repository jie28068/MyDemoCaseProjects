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
	//���캯��
	Snake(Wall &tempWall, Food &tempFood);

	//ö��
	enum{U='w',D='s',L='a',R='d' };

	//��������
	struct Point
	{
		//������
		int x;
		int y;

		//ָ����
		Point* next;
	};
	//��ʼ���ڵ�
	void initSnake();

	//���ٽڵ�
	void deleteSnake();

	//��ӽڵ�
	void addPoint(int x, int y);

	//ɾ���ڵ�
	void delPoint();

	//�ƶ�
	bool move(char k);

	//ͷ���
	Point* head;

	//ά��һ��ǽ
	Wall& wall;

	//ά��һ��ʳ��
	Food& food;

	//β�ͱ�ʶ
	bool isend;

	//��ȡ�߳���
	int count();

	//��ȡˢ��ʱ��
	int sleep();

	//�趨�Ѷ�
	//��ȡ����
	int getscore();
};

#endif // !_SNAKE_HEAD