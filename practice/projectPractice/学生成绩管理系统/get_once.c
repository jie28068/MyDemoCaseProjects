#include <stdio.h>
#include "student.h"
#include <stdlib.h>
int get_once(char* d)
{	/*�õ�ÿһ�еĵ�һ������*/
	int a = 0;
	char* p = d;
	int num = 0;
	while (*p != ' ')
	{
		if (*p > '0' && *p < '9')
			break;
		p++;
	}
	num = atoi(p);//���ַ���ת��Ϊ����
	return num;
}


