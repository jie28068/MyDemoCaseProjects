#include <stdio.h>

/*
	从键盘输入一个字符串。新串是在原串中,每两个字符之间插入一个空格,
	如原串为abcd,则新串为ab cd。要求在函数insert中完成新串的产生﹔并在函数中完成所有相应的输入和输出。
*/
int main52()
{
	int len, i, index = 0;
	char buf[100] = { 0 };
	printf("请输入字符串:");
	gets(buf);
	len = strlen(buf);
	char arr[100] = { 0 };
	char* p = arr;
	for (i = 0; i < len; i++)
	{
		if (i != 0 && i % 2 == 0)//取二整数置空
		{
			p[index] = ' ';
			index++;
		}
		p[index] = buf[i];
		index++;
	}
	p[index] = '\0';
	printf("修改后的字符串:%s\n", p);
	system("pause");
	return 0;
}