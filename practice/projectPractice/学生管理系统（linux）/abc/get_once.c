#include <stdio.h>
#include "student.h"
#include <stdlib.h>
int get_once(char* d)
{	/*得到每一行的第一个整数*/
	int a = 0;
	char* p = d;
	int num = 0;
	while (*p != ' ')
	{
		if (*p > '0' && *p < '9')
			break;
		p++;
	}
	num = atoi(p);//将字符型转换为整型
	return num;
}


