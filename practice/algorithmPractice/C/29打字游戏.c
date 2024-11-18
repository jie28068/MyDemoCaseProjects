#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include <time.h>
void func29(char* arry)
{
	srand((unsigned int)time(NULL));
	/*生成随机大小写字母*/
	for (int i = 0; i < 20; i++)
	{
		/*生成随机数0,1*/
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
		/*等待从键盘得到一个字符*/
		ch = _getch();
		/*计时开始*/
		if (0 == i)
		{
			star_time = time(NULL);
		}
		/*正确输出，计数器加一*/
		if (arry[i] == ch)
		{
			printf("%c", arry[i]);
			count++;
		}
		/*不相同输出_*/
		else
		{
			printf("_");
		}
	}
	/*计时结束*/
	end_time = time(NULL);
	printf("\n");
	printf("正确率为：%.f%%\n", count * 1.0 / 20 * 100);
	printf("耗时%d秒\n", end_time - star_time);
}
void muen29()
{
	printf("******打字游戏******\n");
	printf("------按随机键开始---\n");
	printf("------按ESC键退出----\n");
	/*从键盘得到一个字符*/
	char ch = _getch();
	/*若这个字符是ESC键，就退出*/
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
/*输入字符无需按回车键
	a)如果是Linux平台，拷贝mygetch.h和mygetch.c到所写代码目录中
	#include "mygetch.h"
	char ch = mygetch(); //mygetch()在mygetch.c内部实现，不是标准函数

	b)如果是Windows平台，WINAPI自带类似功能函数
	char ch = _getch(); //需要头文件#include <conio.h>*/