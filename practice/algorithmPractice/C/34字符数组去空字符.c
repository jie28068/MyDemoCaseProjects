#include <stdio.h>

void To_empty(char* Str)
{
	char* p = Str;
	while (*Str != '\0')
	{
		if (*Str != ' ')
		{
			*p++ = *Str;     //如果是空之后就把他的值与后一个值进行交换
		}
		Str++;
	}
	*p = '\0';  //循环结束后加一个斜杠0作为结束符
}

int main34()
{
	char arr[100];
	printf("请输入一个字符串\n");
	fgets(arr, sizeof(arr), stdin);
	To_empty(arr);
	printf("去空后的字符串为：%s\n",arr);
	system("pause");
	return 0;
}
