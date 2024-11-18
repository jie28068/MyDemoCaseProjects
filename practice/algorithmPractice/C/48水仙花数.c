#include <stdio.h>
/*
打印100-1000所有的“水仙花数”，所谓“水仙花数”是指一个三位正整数，
其各位数字的立方和等于该数本身。
思路：
	水仙花数是指一个3位数，它的每个位上的数字的3次幂之和等于它本身
	例如：1^3+5^3+3^3=153
*/
int main48()
{
	int arr = 0;
	int buf = 0;
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			for (int k = 0; k < 10; k++)
			{
				arr = i * i * i + j * j * j + k * k * k;
				buf = 100 * i + 10 * j + k;
				if ((buf == arr) && buf > 100)
				{
					printf("%d\n", buf);
				}
			}
/*方法二*/
	//for (int i = 0; i < 1000; i++)
	//{
	//	//将一个三位数拆解成个位 十位 百位
	//	int a = 0, b = 0, c = 0;
	//	//百位
	//	a = i / 100;
	//	//十位 
	//	b = i / 10 % 10;
	//	//个位
	//	c = i % 10;
	//	if (a * a * a + b * b * b + c * c * c == i)
	//	{
	//		printf("%d是水仙花\n", i);
	//	}
	//}
	system("pause");
	return 0;
}