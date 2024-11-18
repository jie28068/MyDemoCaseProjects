#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX 10

int main20()
{
	srand((unsigned int)time(NULL));
	int* p =malloc(sizeof(int) * MAX);
	if (NULL == p)
	{
		return 0;
	}
	for (int i = 0; i < MAX; i++)
	{
		p[i] = rand() % 80;
		printf("ÅÅÐòÇ°£º%d\n", p[i]);
	}
	
	for (int i = 0; i < MAX - 1; i++)
	{
		for (int j = 0; j < MAX - i - 1; j++)
		{
			if (p[j] > p[j + 1])
			{	
				//int temp = p[j];
				//p[j] = p[j + 1];
				//p[j + 1] = temp;
				p[j] = p[j] ^ p[j + 1];
				p[j+1]= p[j] ^ p[j + 1];
				p[j] = p[j] ^ p[j + 1];
			}
		}
	}
	printf("-----------------------------------\n");
	for (int i = 0; i < MAX; i++)
	{
		printf("ÅÅÐòºó£º%d\n", p[i]);
	}
	if (p != NULL)
	{
		free(p);
		p = NULL;
	}
	system("pause");
	return 0;
}
