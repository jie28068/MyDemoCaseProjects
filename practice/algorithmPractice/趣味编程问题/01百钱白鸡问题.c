#include <stdio.h>

/*
百钱白鸡问题： 一只公鸡 五毛
			  一只母鸡 三毛
			  三只小鸡 一毛
注意：小鸡除三后是一个整数，这样会导致会有几分之几的小鸡，这不符合实际，
	     所以要除以3.0得到一个浮点型
*/
int main01()
{
	for (int x = 0; x < 21; x++)
	{
		for (int y = 0; y < 34; y++)
		{
			for (int z = 0; z < 101; z++)
			{	

				if (5 * x + 3 * y + z / 3.0 == 100 && x + y + z == 100)
				{
					printf("公鸡%d母鸡%d小鸡%d\n", x, y, z);
				}
			}
		}
	}
	system("pause");
	return 0;
}