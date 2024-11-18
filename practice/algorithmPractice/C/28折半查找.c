#include <stdio.h>
/*折半查找要有序且只能找出一个*/
int main28()
{	
	int a[] = { 13,14,78,90,189,355,189 };
	int key;
	/*0*/
	int min=0;
	/*中间值*/
	int mid = 0;
	/*数组长度*/
	int max = sizeof(a) / sizeof(a[0]);
	scanf_s("%d", &key);
	int flag = 0;
	while (min <= max)
	{
		 mid = (min + max) / 2;
		 /*输入的值大于中间值时，重新对min赋值*/
		if (key > a[mid])
		{
			min = mid + 1;
		}
		/*输入的值小于中间值时，重新对max赋值*/
		else if (key < a[mid])
		{
			max = mid - 1;
		}
		else
		{
			printf("%d,%d", a[mid],mid);
			flag = 1;
			break;
		}
	}
	if (0 == flag)
	{
		printf("没有这个数字\n");
	}
	system("pause");
	return 0;
}