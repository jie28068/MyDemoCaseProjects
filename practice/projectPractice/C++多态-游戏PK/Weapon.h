#pragma once
#include <iostream>
using namespace std;

//����������
class Weapon
{
public:
	//��ȡ�˺�
	virtual int getBaseDamage() = 0;

	//��ȡ��Ѫ
	virtual int getSuckBlood() = 0;

	//��ȡ����
	virtual bool getCril() = 0;

	//��ȡ����
	virtual bool getHold() = 0;

	string m_Name;//��������
	int m_Base;//�����˺�

};