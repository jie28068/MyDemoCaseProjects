#pragma once
#include <iostream>
using namespace std;
#include "Weapon.h"
//С����

class Knife :public Weapon
{
public:
	Knife();

	//��ȡ�˺�
	virtual int getBaseDamage();

	//��ȡ��Ѫ
	virtual int getSuckBlood();

	//��ȡ����
	virtual bool getCril();

	//��ȡ����
	virtual bool getHold();
};