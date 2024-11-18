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
typedef int(p_one)[5]; // 方法一             相当于 typedef int [5] p_one ，p_one为起的别名  
typedef int(*p_two)[5];//方法二
void main66()
{
	int arr[5] = {1,5,9,5,2};
	p_one* p1 = &arr;//方法一   得到整个数组的地址，再对该地址解引用，就是数组对应的数值
	p_two p2 = &arr;//方法二    直接定义为一个指针，得到数组地址
	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		printf(" %d\t", arr[i]);
	}
	printf("\n");
	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		printf(" %d\t", (*p1)[i]);//方法一  对数组解引用
	}
	printf("\n");
	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		printf(" %d\t", *(*p2+i));//方法二  先解引用得到数组名，再解引用得到的是数组对应的值
	}
	int(*p_three)[3] = &arr;//方法三  直接定义
	printf("\n");
	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		printf(" %d\t", *(*p_three+i));//方法二  先解引用得到数组名，再解引用得到的是数组对应的值
	}
	system("pause");
}
