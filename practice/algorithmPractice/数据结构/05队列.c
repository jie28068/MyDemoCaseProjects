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
//����һ�����й���ṹ��
struct queue
{
	//��ͷָ��
	struct node* first;
	//��βָ��
	struct node* last;
	//�ڵ���
	int number;
	
};
//����һ���ڵ�
struct node
{
	//ָ����
	struct node* next;
	//������
	dataptye data;
};
//����һ������
struct queue* init_queue()
{
	struct queue* p = malloc(sizeof(struct queue));
	if (NULL == p)
		return 0;
	//��ʼ��ָ��
	p->first = NULL;
	p->last = NULL;
	p->number = 0;
	return p;
}
//����ʼ��һ������
void deinit_queue(struct queue* p)
{
	if (p)
	{
		free(p);
		p = NULL;

	}
}
//���ض��г���
int queuelen(struct queue* p)
{
	return p->number;
}
//��ȡ����Ԫ���׽ڵ���Ϣ
dataptye gethead(struct queue* p)
{
	return p->first->data;
}
//�ж϶����Ƿ�Ϊ��
int queueempty(struct queue* p)
{
	if (p->first == NULL)
	{
		return 0;
	}
	return 1;
}
//���
bool inqueue(struct queue* p, dataptye data)
{
	//����һ���ڵ�
	struct node* np = malloc(sizeof(struct node));
	if (NULL == np)
		return false;
	np->next = NULL;
	np->data = data;
	//β�嵽������,             ����     <--  <--  <-- 
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
//����
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
