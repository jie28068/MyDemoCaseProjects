#include <stdio.h>

int main19()
{
	char row;
	printf("ÇëÊäÈë×ÖÄ¸:\n");
	scanf_s("%c", &row);
	for (int i = 1; i <= row -64; i++)
	{
		for (int j = 1; j <= row -64- i; j++)
		{
			printf(" ");
		}
		for (int k = 1; k <= i  ; k++)
		{
			printf("%c", 64+k);
		}
		//row--;
		for (int l = 1; l <= i-1; l++)
		{
			//int str[100];
			//str[l - 1] = 64 + l;
			printf("%c", row-l);
			
		}
		printf("\n");

	}

	system("pause");
	return 0;
}
//#include <stdio.h>

//int main()
//{
//	int x = 1, y = 1;
//	char q;
//	for (int a = 0; a < 5; a++)
//	{
//		for (int b = 0; b < (5 - x); b++)
//		{
//			printf(" ");
//		}
//
//		for (q = 'A'; q < ('A' + a + 1); q++)
//		{
//			printf("%c", q);
//		}
//		q--;
//		for (int y = 0; y < a; y++)
//		{
//			q--;
//			printf("%c", q);
//		}
//		printf("\n");
//		x++;
//
//
//	}
//	system("pause");
//	return 0;
//}