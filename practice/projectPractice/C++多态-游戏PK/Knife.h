#pragma once
#include <iostream>
using namespace std;
#include "Weapon.h"
//小刀类

class Knife :public Weapon
{
public:
	Knife();

	//获取伤害
	virtual int getBaseDamage();

	//获取吸血
	virtual int getSuckBlood();

	//获取暴击
	virtual bool getCril();

	//获取定身
	virtual bool getHold();
};