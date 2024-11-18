#include <stdio.h>
/*
问题 ：输入一个日期到1990年1月1日的总天数，用该天数除以5，余数如果得到的是1,2,3
      则打鱼，反之晒网。

分析：要确定是平年还是闰年，可以用输入的年数减去1990得到的差，再去除以4，得到的整数
      就是闰年的次数，这个次数就是比平年多的天数。求月份时，因为月份天数的不均匀做一个
	  天数数组，从而得到天数。
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
	printf("请输入当前日期(日月年)：\n");
	scanf_s("%d%d%d", &sut.day, &sut.month, &sut.year);
	int y = 1990;
	int d = 0;
	/*能被4整除而不被100整除的即为闰年*/
	if (((sut.year % 4) == 0) && ((sut.year % 100) != 0))
	{
		int m[13] = { 0,31,29,31,30,31,30,31,31,30,31,31 };//闰年二月就为28天
		int i = sut.year - y;
		int  j = i / 4;
		/*求出月份*/
		for (int e = 0; e < sut.month; e++)
		{
			d += m[e];
		}
		sut.all_day = i * 365 + j + sut.day + d;

	}
	/*反之为平年*/
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
	printf("相隔天数%d\n", sut.all_day);
	int dd = sut.all_day % 5;
	if (dd < 4)
	{
		printf("打鱼");
	}
	else
	{
		printf("晒网");
	}
	system("pause");
	return 0;
}