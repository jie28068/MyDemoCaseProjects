#pragma once
#include <iostream>
using namespace std;
#include "Weapon.h"
//��������

class Tulong :public Weapon
{
public:
	Tulong();
	//��ȡ�˺�
	virtual int getBaseDamage();

	//��ȡ��Ѫ
	virtual int getSuckBlood();

	//��ȡ����
	virtual bool getCril();

	//��ȡ����
	virtual bool getHold();

	//��Ѫ�� ������  ������
	int suckRate;
	int holdRate;
	int crilRate;

	//����
	bool isT(int rate);

};