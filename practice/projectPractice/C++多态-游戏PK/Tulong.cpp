#include "Tulong.h"
#include <time.h>
Tulong::Tulong()
{
	this->m_Base = 20;
	this->m_Name = "ÍÀÁúµ¶";
	this->crilRate = 40;
	this->holdRate = 10;
	this->suckRate = 30;
}

int Tulong::getBaseDamage()
{
	return this->m_Base;
}

int Tulong::getSuckBlood()
{	
	if (isT(this->suckRate))
	{
		return this->m_Base * 0.5;
	}
	return 0;
}

bool Tulong::getCril()
{
	if (isT(this->crilRate))
	{
		return true;
	}
	return false;
}

bool Tulong::getHold()
{
	if (isT(this->holdRate))
	{
		return true;
	}
	return false;
}

bool Tulong::isT(int rate)
{	
	srand((unsigned int)time(NULL));
	int num = rand() % 100 + 1;
	if (num < rate)
	{
		return true;
	}
	return false;
}
