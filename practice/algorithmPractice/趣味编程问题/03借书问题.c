#include <stdio.h>
/*
	问题：小明有五本书，借给A，B，C三个人，每个人每次只能借一本书有多少种借法
	分析：一开始相同过将人编号，发现不好使。所以就将书编号，只要我每次把不同编号的书借出去了
	     另一个人就借不到相同的编号书，这样就实现了目标效果。再利用一个计数位将方式累计。
*/
int main03()
{	
	int count = 0;
	for (int a = 1; a <= 5; a++)
	{
		for (int b = 1; b <= 5; b++)
		{
			for (int c = 1; c <= 5;c++)
			{
				if (a != b && a != c && c != b)
				{	
					//if(a + b + c == 5)
					printf("A:%d\tB:%d\tC:%d\t", a, b, c);
					count++;
					if (count %5 == 0)
					{
						printf("\n");
					}
				}
			}
		}

	}
	printf("有%d种方案\n", count);
	system("pause");
	return 0;
}