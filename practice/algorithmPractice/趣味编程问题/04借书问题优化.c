#include <stdio.h>
/*
	问题：小明有五本书，借给A，B，C三个人，每个人每次只能借一本书有多少种借法
	分析：当前A和B要借的书号相同时，那么C就没有必要继续循环，所以就提高了了效率
	     即加上条件a!=b到c的循环中。
*/
int main04()
{
	int count = 0;
	for (int a = 1; a <= 5; a++)
	{
		for (int b = 1; b <= 5; b++)
		{
			for (int c = 1; c <= 5&&a!=b; c++)
			{
				if ( a != c && c != b)
				{
					//if(a + b + c == 5)
					printf("A:%d\tB:%d\tC:%d\t", a, b, c);
					count++;
					if (count % 5 == 0)
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