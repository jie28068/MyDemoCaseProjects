#include <stdio.h>
/*
	�Ż���ʹ��רҵ���Ӣ����������װ�������ܣ����߲�θУ��Ż��߼�
*/
typedef struct date           //ע��typedef
{
	int year;
	int day;
	int month;

}DATE;

extern int CountDay(DATE);
extern int runyear(int);

void main06()
{
	DATE taday;              //���ṹ��ȡ�˱���,DATE��һ������������
	int totalDay;
	int result;
	printf("��ָ�����ڣ��� �� �գ�\n");
	scanf_s("%d%d%d", &taday.year, &taday.month, &taday.day);
	totalDay = CountDay(taday);   //�����������ĺ���
	result = totalDay % 5;
	printf("%d\n",totalDay);
	if (result > 0 && result < 4)
	{
		printf("�������");
	}
	else
	{
		printf("����ɹ��");
	}
	system("pause");
}

int CountDay(DATE currentDay)
{
	int perMonth[13] = { 0,31,28,31,30,31,30,31,31,30,31,31 };//����һ���·�����������
	int totalDay = 0, year, i;
	for (year = 1990; year < currentDay.year; year++)
	{
		if (runyear(year))        //���ж��Ƿ�Ϊ����ĺ���
			totalDay += 366;
		else
			totalDay += 365;
	}
	if (runyear(currentDay.year))
		perMonth[2] += 1;
	for (i = 0; i < currentDay.month; i++)
		totalDay += perMonth[i];
	totalDay += currentDay.day;
	return totalDay;
}
int runyear(int year)
{
	if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


