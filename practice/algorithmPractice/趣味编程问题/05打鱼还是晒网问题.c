#include <stdio.h>
/*
���� ������һ�����ڵ�1990��1��1�յ����������ø���������5����������õ�����1,2,3
      ����㣬��֮ɹ����

������Ҫȷ����ƽ�껹�����꣬�����������������ȥ1990�õ��Ĳ��ȥ����4���õ�������
      ��������Ĵ���������������Ǳ�ƽ�������������·�ʱ����Ϊ�·������Ĳ�������һ��
	  �������飬�Ӷ��õ�������
*/
struct date
{
	int year;
	int month;
	int day;
	int all_day;

}sut;

int main05()
{
	printf("�����뵱ǰ����(������)��\n");
	scanf_s("%d%d%d", &sut.day, &sut.month, &sut.year);
	int y = 1990;
	int d = 0;
	/*�ܱ�4����������100�����ļ�Ϊ����*/
	if (((sut.year % 4) == 0) && ((sut.year % 100) != 0))
	{
		int m[13] = { 0,31,29,31,30,31,30,31,31,30,31,31 };//������¾�Ϊ28��
		int i = sut.year - y;
		int  j = i / 4;
		/*����·�*/
		for (int e = 0; e < sut.month; e++)
		{
			d += m[e];
		}
		sut.all_day = i * 365 + j + sut.day + d;

	}
	/*��֮Ϊƽ��*/
	else
	{
		int m[13] = { 0,31,28,31,30,31,30,31,31,30,31,31 };
		int i = sut.year - y;
		int  j = i / 4;
		for (int e = 0; e < sut.month; e++)
		{
			d += m[e];
		}
		sut.all_day = i * 365 + j + sut.day + d;
	}
	printf("�������%d\n", sut.all_day);
	int dd = sut.all_day % 5;
	if (dd < 4)
	{
		printf("����");
	}
	else
	{
		printf("ɹ��");
	}
	system("pause");
	return 0;
}