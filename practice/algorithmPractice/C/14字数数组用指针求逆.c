#include<stdio.h>


char* abc(char* arry, int len)
{
	char* p = arry;
	char* p1 = p + len-1;
	char temp = NULL;
	int count = 0;
	while (p < p1)
	{	
		temp = *p;
		*p = *p1;
		*p1 = temp;
		 p++;
		 p1--;
		 count++;

	}
	return p - count;
}
int main14()
{
	//char c1, c2;
	//scanf_s("%d", &c1);
	//c2 = c1 + 9;
	//printf("%c%d\n", c1, c2);
	//printf("%d\n", ~(-2));
	//7个人7天喝7桶水，21个人21天喝多少桶水
	//int a = 0xfd;
	//a = a & (1 << 3);
	//printf("%d\n", a);
	//printf("%d,%u\n",~5,~5);
	//printf("%d,%u\n", ~(-5), ~(-5));
	char arry[] = "hello world";
	//char arry[100];
	//scanf("%s", &arry);
	//fgets(arry, sizeof(arry), stdin);
	int len = sizeof(arry) / sizeof(arry[0])-1 ;
	printf("%s\n", arry);
	char* p = abc(arry, len);
	printf("%s\n", p);
	//system("pause");
	return 0;
}
