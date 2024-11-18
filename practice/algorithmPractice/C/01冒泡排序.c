#include <stdio.h>

int main1()
{
	//int aryy[] = { 54,158,98,3,77 };
	int aryy[5] = { 0 };
	for (int i = 0; i < 5; i++)
	{
		scanf_s("%d", &aryy[i]);
	}
	for (int i = 0; i < sizeof(aryy) / sizeof(aryy[0]) - 1; i++)
	{
		for (int j = 0; j < sizeof(aryy) / sizeof(aryy[0]) - i - 1; j++)
		{
			if (aryy[j] > aryy[j + 1])
			{
				int temp = aryy[j];
				aryy[j] = aryy[j + 1];
				aryy[j + 1] = temp;
			}
		}
	}
	for (int k = 0; k < 5; k++)
	{
		printf("%d\n", aryy[k]);
	}
	system("pause");
	return 0;
}
