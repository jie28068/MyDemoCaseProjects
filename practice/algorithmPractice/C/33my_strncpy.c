#include <stdio.h>

/*将输入字符赋值到另一字符数组中*/
char* My_StrNcpy(char* dest, char* src, int n)
{
	for (int i = 0; i < n; i++)
	{
		dest[i] = src[i];
	}
	dest[n] = '\0';
	return dest;
}

/*递归计算输入字符串的长度*/
int my_strlen(char* str)
{
	if (*str == '\0')   
		return 0;	
	else
		return 1 + my_strlen(str + 1);
}

int main33()
{
	char dest[100];
	char src[100] = { 0 };
	int n = 0;
	printf("请输入字符窜：\n");
	fgets(src, sizeof(src), stdin);
	printf("请输入要复制的长度：\n");
	scanf_s("%d", &n);
	int len = my_strlen(src);
	if (n <= len && n >= 0)
	{
		char* p = My_StrNcpy(dest, src, n);
		printf("复制后的字符为：%s\n", p);
	}
	else
	{
		printf("请输入正确的数字！");
	}

	system("pause");
	return 0;
}