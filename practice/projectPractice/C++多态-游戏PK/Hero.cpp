#include "Hero.h"

Hero::Hero()
{
	this->m_Atk = 50;
	this->m_Def = 50;
	this->m_Hp = 500;
	this->m_Name = "江浩";
	this->weapon = NULL;

}

void Hero::EuiqWeapon(Weapon* weapon)
{
	this->weapon = weapon;
	cout << "英雄:" << this->m_Name << "装备了<<" << this->weapon->m_Name << ">>武器" << endl;
}

void Hero::Attack(Monster* monster)
{
	int damage = 0;
	int addhelp = 0;
	bool isHold = false;
	bool isCril = false;
	if (this->weapon == NULL)
	{
		damage = this->m_Atk;
	}
	else
	{
		//基础伤害
		damage = this->m_Atk + this->weapon->getBaseDamage();

		//计算吸血
		addhelp = this->weapon->getSuckBlood();

		//计算定身
		isHold = this->weapon->getHold();

		//计算暴击
		isCril = this->weapon->getCril();

	}
	if (isCril)
	{
		damage = damage * 2;
		cout << "触发暴击，伤害翻倍" << damage << endl;
	}
	if (isHold)
	{
		cout << "触发定身效果，停止攻击一回" << endl;
	}
	if (addhelp > 0)
	{
		cout << "触发吸血效果" << addhelp << endl;
	}

	//怪物定身
	monster->m_Hold = isHold;

	//计算真时伤害
	int trueDamage = (damage - monster->m_Def) > 0 ? damage - monster->m_Def : 1;
	monster->m_Hp -= trueDamage;

	this->m_Hp += addhelp;
	cout << "英雄" << this->m_Name << "攻击了" << monster->m_Name << "造成了" << trueDamage << "的伤害" << endl;

	
}
