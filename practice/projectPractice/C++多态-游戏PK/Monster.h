#pragma once
#include "Weapon.h"
#include "Hero.h"
//怪物类
class Hero;
class Monster
{
public:
	Monster();
	string m_Name;//名

	int m_Atk;//攻击力

	int m_Def;//防御力

	int m_Hp;//血量

	bool m_Hold;

	void Attack(Hero* hero);
};