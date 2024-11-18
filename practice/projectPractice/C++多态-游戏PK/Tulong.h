#pragma once
#include <iostream>
using namespace std;
#include "Weapon.h"
//屠龙刀类

class Tulong :public Weapon
{
public:
	Tulong();
	//获取伤害
	virtual int getBaseDamage();

	//获取吸血
	virtual int getSuckBlood();

	//获取暴击
	virtual bool getCril();

	//获取定身
	virtual bool getHold();

	//吸血率 定身率  暴击率
	int suckRate;
	int holdRate;
	int crilRate;

	//几率
	bool isT(int rate);

};