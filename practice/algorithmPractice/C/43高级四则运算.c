#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
enum MyEnum            //ö��ѡ��
{
	add, cot, mlt, dive
}opt;
int main43()
{
	printf("������ʽ��......");
	srand((unsigned int)time(NULL));
	FILE* cp = fopen("../../a.txt", "w");  //��ַ�������ڱ�
	if(!cp)
	{
		return -1;
	}
	char c=0;
	int a, b;
	char buf[100];
	for (int i = 0; i < 100; i++)
	{
		memset(buf, 0, 100);
		switch (rand() % 4)
		{
		case add:
			c = '+';
			break;
		case cot:
			c = '-';
			break;
		case mlt:
			c = '*';
			break;
		case dive:
			c = '/';
			break;
		default:
			break;
		}
		a = rand() % 10 + 1;
		b = rand() % 10 + 1;
		sprintf(buf, "%d%c%d=\n", a, c, b);  //��ʽ������
		int j = 0;
		while(buf[j])   //�����е����ݴ��룬֪����\0������
		fputc(buf[j++], cp);
	}

	fclose(cp);
	system("pause");
	return 0;
}