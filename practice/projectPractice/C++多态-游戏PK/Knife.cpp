#include "Knife.h"

Knife::Knife()
{
	this->m_Base = 10;
	this->m_Name = "С��";
}

int Knife::getBaseDamage()
{
	return this->m_Base;
}

int Knife::getSuckBlood()
{
	return 0;
}

bool Knife::getCril()
{
	return false;
}

bool Knife::getHold()
{
	return false;
}
