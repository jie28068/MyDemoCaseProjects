#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include <time.h>
void func29(char* arry)
{
	srand((unsigned int)time(NULL));
	/*���������Сд��ĸ*/
	for (int i = 0; i < 20; i++)
	{
		/*���������0,1*/
		int j = rand() % 2;
		if (0 == j)
		{
			arry[i] = rand() % 26 + 'a';
		}
		else
		{
			arry[i] = rand() % 26 + 'A';
		}
		printf("%c", arry[i]);
	}
	printf("\n");
}
void star29(char* arry)
{
	int star_time;
	int end_time;
	char ch = 0;
	int count = 0;
	for (int i = 0; i < 20; i++)
	{
		/*�ȴ��Ӽ��̵õ�һ���ַ�*/
		ch = _getch();
		/*��ʱ��ʼ*/
		if (0 == i)
		{
			star_time = time(NULL);
		}
		/*��ȷ�������������һ*/
		if (arry[i] == ch)
		{
			printf("%c", arry[i]);
			count++;
		}
		/*����ͬ���_*/
		else
		{
			printf("_");
		}
	}
	/*��ʱ����*/
	end_time = time(NULL);
	printf("\n");
	printf("��ȷ��Ϊ��%.f%%\n", count * 1.0 / 20 * 100);
	printf("��ʱ%d��\n", end_time - star_time);
}
void muen29()
{
	printf("******������Ϸ******\n");
	printf("------���������ʼ---\n");
	printf("------��ESC���˳�----\n");
	/*�Ӽ��̵õ�һ���ַ�*/
	char ch = _getch();
	/*������ַ���ESC�������˳�*/
	if (ch == 27)
	{
		exit(0);
	}
}
int main29(void)
{
	char arry[21] = { 0 };
	while (1)
	{
		muen29();
		func29(arry);
		star29(arry);
	}
	//system("pause");
	return 0;
}
/*�����ַ����谴�س���
	a)�����Linuxƽ̨������mygetch.h��mygetch.c����д����Ŀ¼��
	#include "mygetch.h"
	char ch = mygetch(); //mygetch()��mygetch.c�ڲ�ʵ�֣����Ǳ�׼����

	b)�����Windowsƽ̨��WINAPI�Դ����ƹ��ܺ���
	char ch = _getch(); //��Ҫͷ�ļ�#include <conio.h>*/