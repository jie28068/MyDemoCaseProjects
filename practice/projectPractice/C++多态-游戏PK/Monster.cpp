#include "Monster.h"

Monster::Monster()
{
	this->m_Hp = 300;
	this->m_Atk = 70;
	this->m_Def = 40;
	this->m_Hold = false;
	this->m_Name = "ç÷ç÷¹Ö";
}

void Monster::Attack(Hero* hero)
{
	if (this->m_Hold)
	{
		cout << "¶¨ÉíLÁË" << this->m_Name << endl;
		return;
	}
	//¼ÆËã¹¥»÷ÉËº¦
	int damage = (this->m_Atk - hero->m_Def) > 0 ? this->m_Atk - hero->m_Def : 1;
	hero->m_Hp -= damage;
	cout << "¹ÖÎï" << this->m_Name << "¹¥»÷ÁËÓ¢ÐÛ" << hero->m_Name << "Ôì³ÉÁËÉËº¦" << damage << endl;

}
