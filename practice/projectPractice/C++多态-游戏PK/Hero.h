#pragma once
#include <iostream>
using namespace std;
#include "Weapon.h"
#include "Monster.h"

class Monster;
//人类
class Hero
{
public:
	Hero();

	string m_Name;//人名

	int m_Atk;//攻击力

	int m_Def;//防御力

	int m_Hp;//血量
	
	Weapon* weapon;//武器

	void EuiqWeapon(Weapon* weapon);//装备武器

	void Attack(Monster* monster);//攻击
};