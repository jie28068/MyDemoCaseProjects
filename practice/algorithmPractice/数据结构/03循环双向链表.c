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
//定义节点结构体
struct node
{	
	struct node* pev;
	int data;
	struct node* next;
};
//定义头节点结构体
struct headnode
{
	struct node* first;
	struct node* last;
	int number;
};
//创建链表
struct headnode* changed3()
{
	//定义一个头结点
	struct headnode* head = malloc(sizeof(struct headnode));
	if (NULL == head)
		return;
	//初始化节点
	head->first = NULL;
	head->last = NULL;
	head->number = 0;
	//节点赋值
	while (1)
	{
		int a;
		scanf("%d", &a);
		if (0 == a)
			break;
		//定义一个节点
		struct node* p = malloc(sizeof(struct node));
		if (NULL == p)
			return;
		//初始化节点
		p->next = NULL;
		p->pev = NULL;
		p->data = a;
		//插入链表
		if (head->first == NULL)
		{
			head->first = p;
			head->last = p;
		}
		//不为空的情况，尾插
		else
		{
			head->last->next = p;
			p->pev = head->last;
			head->last = p;
			head->first->pev = head->last;
			head->last->next = head->first;
		}
		head->number++;
	}
	return head;
}
//打印
void put3(struct headnode* head)
{
	int n = head->number;
	
		while (n--)
		{
			printf("%d", head->first->data);
			head->first = head->first->next;
		}
}
//n-- 与--n
void dj()
{
	int n = 2,nn=2;
	//运行俩次
	while (n--)
	{
		printf("*");
	}
	//运行一次
	while (--nn)
	{
		printf("-");
	}
}
//添加
struct headnode* add3(struct headnode* head ,int len ,int a)
{
	//定义新的节点
	struct node* pnew = malloc(sizeof(struct node));
	if (NULL == pnew)
	return;
	//
	struct node* p = head->first;
	//初始化节点
	pnew->next = NULL;
	pnew->data = a;
	pnew->pev = NULL;
	int n = head->number;
	//遍历链表
	while (n--)
	{
		if (len == p->data && len != head->last->data)
		{
			pnew->pev = p;
			pnew->next = p->next;
			p->next->pev = pnew;
			p->next = pnew;
			head->number++;
		}
		else if (len == p->data)
		{
			//节点在尾部,尾插
			p->next = pnew;
			pnew->pev = p;
			pnew->next = head->first;
			head->first->pev = pnew;
			head->last = pnew;		
			head->number++;
		}
		p = p->next;
	}
	return head;
}
int main03()
{
	struct headnode* head = changed3();
	head = add3(head, 3, 8);
	put3(head);
	//dj();
	system("pause");
	return 0;
}
