#include <stdio.h>
/*
	输入任意正整数，编程判断该数是否为回文数
	(回文数是指从左到右读与从右到左读一样，如12321)。
*/
int main49()
{
	char buf[10] = { 0 };
	char arr[10] = { 0 };
	int j = 0;
	int i = 0;
	fgets(buf, sizeof(buf), stdin);   
	for (int i = strlen(buf) - 1; i >= 0; i--)  //将输入的数字求逆，然后在再将这俩数字比较
	{
			arr[j] = buf[i];
			j++;
	}
	for (i = 0; i < strlen(arr); i++)
	{
		printf("%c", arr[i]);
	}
	arr[i] = '\n';    //注意输入时的回车问题
	printf("\n");
	char* p1 = buf;
	char* p2 = arr;
	if (*p1 == *(p2+1))
	{
		printf("是回文数\n");
	}
	else
	{
		printf("不是回文数\n");
	}
	system("pause");
	return 0;
}