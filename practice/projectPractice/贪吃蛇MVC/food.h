#pragma once
#ifndef _FOOD_HEAD
#define _FOOD_HEAD
#include <iostream>
using namespace std;
#include "wall.h"

class Food
{
public:
	void setFood();

	Food(Wall& tempWall);
	int foodx;
	int foody;

	//ǽ
	Wall& wall;
};


#endif // !_FOOD_HEAD
