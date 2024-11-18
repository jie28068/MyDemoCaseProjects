#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */

int main55()
{
	//int a[12] = { 1,2,3,4,5,6,7,8,9,10,11,12 };
	//	printf("第一题：%d %d %d \n", *a, *(a + 1), *(&a + 1));//首地址，第二元素地址，第二个元素地址

	//	int b[3][4] = { 1,2,3,4,5,6,7,8,9,10,11,12 };
	//	printf("第二题：%p %p %p %p \n", b, b + 1, &b + 1, b[1] + 1);//0，

	//	int c[5] = { 1,2,3,4,5 };
	//	int* pr;
	//	pr = (int*)(&c + 1);
		//printf("第三题：%d\n", *(pr - 1));//5

		int d[5] = { 1,2,3,4,5 };
		int* p;
		p = (int*)&d + 1;
		printf("第四题：%d\n", *(p - 1));//1
	system("pause");
	return 0;
}
