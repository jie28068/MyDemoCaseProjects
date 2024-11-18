#include "Hero.h"

Hero::Hero()
{
	this->m_Atk = 50;
	this->m_Def = 50;
	this->m_Hp = 500;
	this->m_Name = "����";
	this->weapon = NULL;

}

void Hero::EuiqWeapon(Weapon* weapon)
{
	this->weapon = weapon;
	cout << "Ӣ��:" << this->m_Name << "װ����<<" << this->weapon->m_Name << ">>����" << endl;
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
		//�����˺�
		damage = this->m_Atk + this->weapon->getBaseDamage();

		//������Ѫ
		addhelp = this->weapon->getSuckBlood();

		//���㶨��
		isHold = this->weapon->getHold();

		//���㱩��
		isCril = this->weapon->getCril();

	}
	if (isCril)
	{
		damage = damage * 2;
		cout << "�����������˺�����" << damage << endl;
	}
	if (isHold)
	{
		cout << "��������Ч����ֹͣ����һ��" << endl;
	}
	if (addhelp > 0)
	{
		cout << "������ѪЧ��" << addhelp << endl;
	}

	//���ﶨ��
	monster->m_Hold = isHold;

	//������ʱ�˺�
	int trueDamage = (damage - monster->m_Def) > 0 ? damage - monster->m_Def : 1;
	monster->m_Hp -= trueDamage;

	this->m_Hp += addhelp;
	cout << "Ӣ��" << this->m_Name << "������" << monster->m_Name << "�����" << trueDamage << "���˺�" << endl;

	
}
