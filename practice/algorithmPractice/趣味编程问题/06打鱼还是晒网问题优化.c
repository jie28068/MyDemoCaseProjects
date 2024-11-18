#include <stdio.h>
/*
	优化：使用专业点的英文命名，封装函数功能，更具层次感，优化逻辑
*/
typedef struct date           //注意typedef
{
	int year;
	int day;
	int month;

}DATE;

extern int CountDay(DATE);
extern int runyear(int);

void main06()
{
	DATE taday;              //给结构体取了别名,DATE是一种数据类型了
	int totalDay;
	int result;
	printf("请指定日期（年 月 日）\n");
	scanf_s("%d%d%d", &taday.year, &taday.month, &taday.day);
	totalDay = CountDay(taday);   //调用求天数的函数
	result = totalDay % 5;
	printf("%d\n",totalDay);
	if (result > 0 && result < 4)
	{
		printf("今天打鱼");
	}
	else
	{
		printf("今天晒网");
	}
	system("pause");
}

int CountDay(DATE currentDay)
{
	int perMonth[13] = { 0,31,28,31,30,31,30,31,31,30,31,31 };//定义一个月份天数的数组
	int totalDay = 0, year, i;
	for (year = 1990; year < currentDay.year; year++)
	{
		if (runyear(year))        //调判断是否为闰年的函数
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


