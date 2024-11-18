#if 1
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "Hero.h"
#include "Knife.h"
#include "Monster.h"
#include "Tulong.h"
#include "Weapon.h"
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
void play()
{
	//创建怪物
	Monster* monster = new Monster;
	//创建武器
	Weapon* kinfe = new Knife;
	Weapon* tulong = new Tulong;
	//创建英雄
	Hero* hero = new Hero;
	//让用户选择武器
	cout << "请选择武器" << endl;
	cout << "1.空手" << endl;
	cout << "2.小刀" << endl;
	cout << "3.屠龙刀" << endl;
	int op;
	cin >> op;
	switch (op)
	{
	case 1:
		cout << "打不过！" << endl;
		break;
	case 2:
		hero->EuiqWeapon(kinfe);
		break;
	case 3:
		hero->EuiqWeapon(tulong);
		break;
	default:
		break;
	}
	getchar();
	int round = 1;
	while (true)
	{
		getchar();
		system("cls");
		cout << "***当前第" << round << "回合开始***" << endl;
		if (hero->m_Hp <= 0)
		{
			cout << "英雄" << hero->m_Name << "已挂，游戏结束" << endl;
			break;
		}
		hero->Attack(monster);
		if (monster->m_Hp <= 0)
		{
			cout << "怪物" << monster->m_Name << "已挂，顺利通关" << endl;
			break;
		}
		monster->Attack(hero);
		if (hero->m_Hp <= 0)
		{
			cout << "英雄" << hero->m_Name << "已挂，游戏结束" << endl;
			break;
		}
		cout << "英雄剩余血量" << hero->m_Hp << endl;
		cout << "怪物剩余血量" << monster->m_Hp << endl;
		round++;
	}
	delete monster;
	delete hero;
	delete kinfe;
	delete tulong;
}
int main()
{
	
	play();
	system("pause");
	return  EXIT_SUCCESS;
}
#endif;
