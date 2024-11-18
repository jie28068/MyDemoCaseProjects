#include <stdio.h>
/*增加函数的易用性*/
typedef int (*p)(int a, int b);//换层皮

int max(int a, int b)     //函数名其实也是以一个地址
{
	return a > b ? a : b;
}

int main37()
{
	p q = max;      //p就是一个数据类型了
	int j = q(1, 2);
	printf("%d\n", j);
	system("pause");
	return 0;
}