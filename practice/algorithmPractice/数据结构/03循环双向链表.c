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
//����ڵ�ṹ��
struct node
{	
	struct node* pev;
	int data;
	struct node* next;
};
//����ͷ�ڵ�ṹ��
struct headnode
{
	struct node* first;
	struct node* last;
	int number;
};
//��������
struct headnode* changed3()
{
	//����һ��ͷ���
	struct headnode* head = malloc(sizeof(struct headnode));
	if (NULL == head)
		return;
	//��ʼ���ڵ�
	head->first = NULL;
	head->last = NULL;
	head->number = 0;
	//�ڵ㸳ֵ
	while (1)
	{
		int a;
		scanf("%d", &a);
		if (0 == a)
			break;
		//����һ���ڵ�
		struct node* p = malloc(sizeof(struct node));
		if (NULL == p)
			return;
		//��ʼ���ڵ�
		p->next = NULL;
		p->pev = NULL;
		p->data = a;
		//��������
		if (head->first == NULL)
		{
			head->first = p;
			head->last = p;
		}
		//��Ϊ�յ������β��
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
//��ӡ
void put3(struct headnode* head)
{
	int n = head->number;
	
		while (n--)
		{
			printf("%d", head->first->data);
			head->first = head->first->next;
		}
}
//n-- ��--n
void dj()
{
	int n = 2,nn=2;
	//��������
	while (n--)
	{
		printf("*");
	}
	//����һ��
	while (--nn)
	{
		printf("-");
	}
}
//���
struct headnode* add3(struct headnode* head ,int len ,int a)
{
	//�����µĽڵ�
	struct node* pnew = malloc(sizeof(struct node));
	if (NULL == pnew)
	return;
	//
	struct node* p = head->first;
	//��ʼ���ڵ�
	pnew->next = NULL;
	pnew->data = a;
	pnew->pev = NULL;
	int n = head->number;
	//��������
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
			//�ڵ���β��,β��
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
