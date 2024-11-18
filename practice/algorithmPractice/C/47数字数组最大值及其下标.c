#include <stdio.h>
//#define _CRT_SECURE_NO_WARNINGS
//#define _CRT_SECURE_NO_WARNINGS

/*
	键盘输入12个数存入二维数组a[3][4]中，
	编写程序求出最大元素的值及它所在的行号和列号。
*/
int main47()
{
	int a[3][4];
	int temp = 0;
	int i1 = 0;
	int j1 = 0;
	for(int i=0;i<3;i++)
		for(int j=0;j<4;j++)
			scanf_s("%d", &a[i][j]);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 4; j++)
			if (a[i][j] > temp)
			{
				temp = a[i][j];
				i1 = i;
				j1 = j;
			}				
	printf("最大的是：%d\n下标为:%d,%d\n", temp, i1, j1);
	system("pause");
	return 0;
}