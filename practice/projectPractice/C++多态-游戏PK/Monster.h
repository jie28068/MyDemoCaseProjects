#pragma once
#include "Weapon.h"
#include "Hero.h"
//������
class Hero;
class Monster
{
public:
	Monster();
	string m_Name;//��

	int m_Atk;//������

	int m_Def;//������

	int m_Hp;//Ѫ��

	bool m_Hold;

	void Attack(Hero* hero);
};