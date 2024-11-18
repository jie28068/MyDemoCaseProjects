#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <stdbool.h>
#define size 1024
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
typedef int datapef;
//定义栈的结构体
typedef struct mystack
{
	int arr[size];//用数组方式存放
	int top;//数组下标
}ss;
//初始化栈
ss* changed_stack()
{
	ss* sp = malloc(sizeof(ss));
	if (NULL == sp)
		return 0;
	//初始化栈
	sp->top = -1;
	return sp;
}
//反初始化
void free_stack(ss* sp)
{
	if (NULL != sp)
	{
		free(sp);
		sp = NULL;
	}
}
//清空栈
void clear_stack(ss* sp)
{
	if (sp)
	{
		sp->top = -1;
	}
}
//判断栈是否为空
int empty_stack(ss* sp)
{
	if (sp)
	{
		return sp->top == -1;
	}
	return -1;
}
//获取栈的长度
int len_stack(ss* sp)
{
	if (sp)
	{
		return sp->top + 1;
	}
	return -1;
}
//获取栈顶元素
datapef getup_stcak(ss* sp)
{
	if (sp)
	{
		return sp->arr[sp->top];
	}
	return -1;
}
//出栈
datapef pop_stack(ss* sp)
{
	if (sp)
	{
		return sp->arr[sp->top--];
	}
	return -1;
}
//入栈
bool push_stack(ss* sp, datapef arr)
{
	if (sp)
	{
		sp->arr[++sp->top] = arr;
		return true;
	}
	return false;
}
//十进制转八进制
int convert(ss* sp,int n)
{
	int temp;
	if (n == 0)
	{	
		printf("八进制：o");
		return 0;
	}
	else
	{
		temp = n % 8;  
		//入栈
		push_stack(sp, temp);
		n = n / 8;   
		convert(sp,n); 
		//出栈
		printf("%d", pop_stack(sp));
	}
	return 0;
}
int main04()
{
	printf("请输入数字：\n");
	ss* p = changed_stack();
	int a = 0;
	scanf("%d", &a);
	convert(p,a);
	//printf("%d", a);
	system("pause");
	return 0;
}
