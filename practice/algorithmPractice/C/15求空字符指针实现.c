//#include <stdio.h>
//
//int a(char*arry,int len)
//{
//	char* p = arry;
//	int count = 0;
//	char* p1 = p + len +1;
//	while (p < p1)
//	{
//		if (*p == ' ')
//		{
//			count++;
//		}
//		p++;
//	}
//	return count;
//}
//int main15()
//{
//
//	char arry[] = { "   hello  world r " };
//	int len = sizeof(arry) / sizeof(arry[0]) - 1;
//	int p = a(arry, len);
//	printf("%d\n", p);
//	system("pause");
//	return 0;
//}