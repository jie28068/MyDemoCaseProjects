#include <stdio.h>

int main27()
{	
	int a[] = { 1,58,54,88,99,5,8,12,64,54,2,100};
	int b = 0;
	for (int i = 0; i < 12; i++)
	{
			printf("%d\n", a[i]);
	}
	printf("请输入一个数字\n");
	scanf_s("%d", &b);
	int len = sizeof(a) / sizeof(a[0]);
	int temp = len;
	int* p = a;
	/*设定一个标志位，用来判断数组遍历完后是否有这个数字*/
	int flag = 0;         
	
	/*指针实现方式*/
	//while (len)
	//{	
	//	if (*p == b)
	//	{				
	//		printf("对应的数组下标为：%d\n", (temp - len));
	//		flag = 1;
	//	}
	//	len--;
	//	p++;
	//}
	/*数组实现方式*/
	for (int i = 0; i < len; i++)
	{
		if (a[i] == b)
		{
			printf("对应的数组下标为：%d\n", i);
			flag = 1;
		}
	}
	if (!flag)
	{
		printf("该数组中没有该数字\n");
	}
	system("pause");
	return 0;
}