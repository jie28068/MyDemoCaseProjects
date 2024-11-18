#include <stdio.h>
/*
	问题：A说车牌前俩位相同，B说后俩位相同但是与前俩位不同，C说车牌号刚好是某位数的平方
	      求出车牌号。
	优化：设置标志位。在找到数字后退出，没有必要在次循环下去。
*/
int main07()
{	
	int k,i,j,temp;
	int falg = 0;
	for (i = 0; i < 10; i++)
	{
		if (falg) 
			break;
		for ( j = 0; j < 10; j++)
		{
			if (falg)
				break;
			if (i != j)
			{
				k = 1000 * i + 100 * i + 10 * j + j;
				for (temp = 31; temp < 99; temp++)
					if (temp * temp == k)
					{
						printf("车牌号为%d\n", k);
						falg = 1;
						break;           //退出最内层循环
					}
					
			}
		}
	}
	system("pause");
	return 0;
}