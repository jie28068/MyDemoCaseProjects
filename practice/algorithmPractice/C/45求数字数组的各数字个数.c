#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
/*
	输入一行数字字符,请用数组元素作为计数器来统计每个数字字符的个数
*/
#include <stdio.h>
int main45()
{
	int i, n, str[10] = { 0 }; char ch;
	printf("请输入一串数字字符：");                
	while ((ch = getchar()) != '\n') //不见得一定要scanf或者fgets得到输入，getchar也可行
	{
		if (ch == '0')
			str[9]++;     //使用数组存放数字出现次数的个数
		else if (ch > '0' && ch <= '9')
			str[ch - 49]++;   //注意这个mcs字符，0的十进制是48，后面的数字加一
	}
	for (i = 0; i < 9; i++)
		printf("%d:%d个\n", i + 1, str[i]);
	printf("0:%d个", str[9]);
	system("pause");
	return 0;
}