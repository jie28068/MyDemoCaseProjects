#pragma once
#include <iostream>
using namespace std;
#include "Weapon.h"
#include "Monster.h"

class Monster;
//����
class Hero
{
public:
	Hero();

	string m_Name;//����

	int m_Atk;//������

	int m_Def;//������

	int m_Hp;//Ѫ��
	
	Weapon* weapon;//����

	void EuiqWeapon(Weapon* weapon);//װ������

	void Attack(Monster* monster);//����
};