#pragma once
#include <iostream>
using namespace std;

//抽象类武器
class Weapon
{
public:
	//获取伤害
	virtual int getBaseDamage() = 0;

	//获取吸血
	virtual int getSuckBlood() = 0;

	//获取暴击
	virtual bool getCril() = 0;

	//获取定身
	virtual bool getHold() = 0;

	string m_Name;//武器名称
	int m_Base;//基础伤害

};