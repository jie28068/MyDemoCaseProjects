#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <stdbool.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
typedef int dataptye;
//定义一个队列管理结构体
struct queue
{
	//对头指针
	struct node* first;
	//队尾指针
	struct node* last;
	//节点数
	int number;
	
};
//定义一个节点
struct node
{
	//指针域
	struct node* next;
	//数据域
	dataptye data;
};
//创建一个队列
struct queue* init_queue()
{
	struct queue* p = malloc(sizeof(struct queue));
	if (NULL == p)
		return 0;
	//初始化指针
	p->first = NULL;
	p->last = NULL;
	p->number = 0;
	return p;
}
//反初始化一个队列
void deinit_queue(struct queue* p)
{
	if (p)
	{
		free(p);
		p = NULL;

	}
}
//返回队列长度
int queuelen(struct queue* p)
{
	return p->number;
}
//获取队列元素首节点信息
dataptye gethead(struct queue* p)
{
	return p->first->data;
}
//判断队列是否为空
int queueempty(struct queue* p)
{
	if (p->first == NULL)
	{
		return 0;
	}
	return 1;
}
//入队
bool inqueue(struct queue* p, dataptye data)
{
	//定义一个节点
	struct node* np = malloc(sizeof(struct node));
	if (NULL == np)
		return false;
	np->next = NULL;
	np->data = data;
	//尾插到队列中,             方向：     <--  <--  <-- 
	if (p->first == NULL)
	{
		p->first = np;
		p->last = np;
	}
	else
	{
		p->last->next = np;
		p->last = np;
	}
	p->number++;
	return true;
}
//出队
dataptye outqueue(struct queue* p)
{
	struct node* np = p->first;
	p->first = p->first->next;
	np->next = NULL;
	return np->data;
}
int main05()
{
	struct queue* p = malloc(sizeof(struct queue));

	system("pause");
	return 0;
}
