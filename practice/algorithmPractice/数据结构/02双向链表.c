#if 0
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
//�ṹ��ڵ�
struct node
{
	struct node* ppev;
	int data;
	struct node* pnext;
};
//�ṹ��ͷ���
struct headnode
{
	struct node* first;
	struct node* last;
	int number;
};
//���ɽڵ�
struct headnode* make2()
{
	//����ͷ���
	struct headnode* head = malloc(sizeof(struct headnode));
	if (NULL == head)
		return;
	//��ʼ��ͷ���
	head->first = NULL;
	head->last = NULL;
	head->number = 0;
	//��������
	while (1)
	{
		int a = 0;
		scanf("%d", &a);
		if (0 == a)
		{
			break;
		}
		//�����½ڵ�
		struct node* pnew = malloc(sizeof(struct node));
		if (NULL == pnew)
			return;
		//���ڵ㸳ֵ
		pnew->data = a;
		pnew->ppev = NULL;
		pnew->pnext = NULL;
		//���ӵ�������
		if (head->first == NULL) //��ʼΪ��
		{
			head->first = pnew;
			head->last = pnew;
		}
		else//��ʼ��Ϊ�գ�β��
		{
			head->last->pnext = pnew;
			pnew->ppev = head->last;
			head->last = pnew;
		}
		head->number++;
	}
	return head;
}
//��ӡ
void show_list(struct headnode* head)
{
	//ת��Ϊ�ڵ�����
	for (struct node* p = head->first; p != NULL; p = p->pnext)
	{
		printf("%d  ", p->data);
	}
	printf("\n");
	printf("������Ľڵ�������Ϊ%d��\n", head->number);
}
//ɾ���ڵ�
struct headnode* delete2(struct headnode *head ,int len)
{
	struct node* pw = malloc(sizeof(struct node));
	if (NULL == pw)
		return;
	pw = head->first;
	//�ȱ����ҵ�Ҫɾ����ֵ 
	while (pw)
	{
		if (pw->data == len)
		{
			break;
		}
		else
		{
			pw = pw->pnext;
		}
	}
	if (head->first == pw) //����ǵ�һ���ڵ�
	{
		head->first = pw->pnext;
		pw->pnext->ppev = NULL;
		pw->pnext = NULL;
		head->number--;
	}
	else if(head->last == pw)//�����β���
	{
		head->last = pw->ppev;
		pw->ppev->pnext = NULL;
		pw->ppev = NULL;
		head->number--;
	}
	else if (pw == NULL)//�Ҳ���ɾ����ֵʱ
	{
		printf("�Ҳ�����\n");
	}
	else//������м��ֵ
	{
		pw->pnext->ppev = pw->ppev;
		pw->ppev->pnext = pw->pnext;
		head->number--;
	}
	return head;
}
//�жϵ�ǰ�����Ƿ������ֵ
int pan(int a, struct headnode * pp)
{
	struct node* p = pp->first;
	while (p)
	{
		if (p->data == a)
		{
			return 0;
		}
		p = p->pnext;
	}
	return 1;
}
//�ҳ�������������ͬ��ֵ
struct headnode* compare(struct headnode* head1, struct headnode* head2)
{
	//����һ���µ�����
	struct headnode* head3 = malloc(sizeof(struct headnode));
	if (NULL == head3)
		return;
	//��ʼ������
	head3->first = NULL;
	head3->last = NULL;
	head3->number = 0;
	//ѭ�������ڵ�
	for (struct node* p1 = head1->first;p1!=NULL;p1=p1->pnext)
	{
		for (struct node* p2 = head2->first; p2 != NULL; p2 = p2->pnext)
		{		
			if (p1->data == p2->data)
			{   
				//����һ����ʱ�ڵ���p2
				struct node* ptemp = malloc(sizeof(struct node));
				if (NULL == ptemp)
					return;
				//��ʼ������
				ptemp->data = p2->data;
				ptemp->pnext = NULL;
				ptemp->ppev = NULL;
				//�ж��Ƿ��Ѿ������ֵ
				int a = pan(ptemp->data, head3);
				//Ϊ��ʱ����ͷ���
				if (head3->first == NULL)
				{
					head3->first = ptemp;
					head3->last = ptemp;
					head3->number++;
				}
				//��Ϊ��ʱ
				else if(a)
				{
					head3->last->pnext = ptemp;
					ptemp->ppev = head3->last;
					head3->last = ptemp;
					head3->number++;
				}
			}
		}
	}
	return head3;
}
//�ͷŶѿռ�
void main02()
{
	struct headnode* head1 = make2();
	//struct headnode* head1 = delete2(head,3);
	//show_list(head1);
	struct headnode* head2 = make2();
	struct headnode* head3 = compare(head1, head2);
	show_list(head3);
	system("pause");
}

#endif
