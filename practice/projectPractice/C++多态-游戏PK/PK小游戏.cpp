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
	//��������
	Monster* monster = new Monster;
	//��������
	Weapon* kinfe = new Knife;
	Weapon* tulong = new Tulong;
	//����Ӣ��
	Hero* hero = new Hero;
	//���û�ѡ������
	cout << "��ѡ������" << endl;
	cout << "1.����" << endl;
	cout << "2.С��" << endl;
	cout << "3.������" << endl;
	int op;
	cin >> op;
	switch (op)
	{
	case 1:
		cout << "�򲻹���" << endl;
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
		cout << "***��ǰ��" << round << "�غϿ�ʼ***" << endl;
		if (hero->m_Hp <= 0)
		{
			cout << "Ӣ��" << hero->m_Name << "�ѹң���Ϸ����" << endl;
			break;
		}
		hero->Attack(monster);
		if (monster->m_Hp <= 0)
		{
			cout << "����" << monster->m_Name << "�ѹң�˳��ͨ��" << endl;
			break;
		}
		monster->Attack(hero);
		if (hero->m_Hp <= 0)
		{
			cout << "Ӣ��" << hero->m_Name << "�ѹң���Ϸ����" << endl;
			break;
		}
		cout << "Ӣ��ʣ��Ѫ��" << hero->m_Hp << endl;
		cout << "����ʣ��Ѫ��" << monster->m_Hp << endl;
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
