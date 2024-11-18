#include <stdio.h>
/*
	编写猜数游戏，程序给定某个整数，从键盘上反复输入数据进行猜测。如
	果未猜中，程序提示输入过大或者过小;如果猜中，则输出猜的次数，最多允许
	猜10次。
*/
int main50()
{
	int a = 0;
	int count = 0;
	while (56 != a)
	{
		scanf_s("%d", &a);
		if (count <= 10)
		{
			if (a > 56)
			{
				printf("大了");
				count++;
			}
			else if (a < 56)
			{
				printf("小了");
				count++;
			}
			else
			{
				count++;
				break;
			}
		}
		else
		{
			printf("次数用完！");
		}
	}
	printf("56!!\n次数：%d",count);
	system("pause");
	return 0;
}