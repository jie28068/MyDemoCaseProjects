#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
///*
//   * User:SJ
//   * Time:
//   * Function:
//   * Explain:
// */
#if 1
//����ڵ�
typedef struct node
{
	int sum;
	struct node* next;
}nd;
//����ͷ�ڵ�ṹ��
typedef struct HeadNode
{
	nd* frist;
	nd* end;
	int number;
}hn;

//�����ڵ�
hn* changed_node()
{
	int a = 0;
	//����ͷ���
	hn* fp = malloc(sizeof(hn));
	if (fp == NULL)
		return;
	fp->frist = NULL;
	fp->end = NULL;
	fp->number = 0;

	while (1)
	{
		//�����µĽڵ�
		nd* pnew = malloc(sizeof(nd));
		nd* temp = malloc(sizeof(nd));
		if (pnew == NULL)
			return;
		//��������
		printf("����������:\n");
		scanf("%d", &a);
		if (-1 == a)
			break;   /*������*/
		//���ڵ㸳ֵ
		pnew->sum = a;
		pnew->next = NULL;
		//�ж��Ƿ�ΪΪ��һ���ڵ�
		if (NULL == fp->frist)
		{
			fp->frist = pnew;
			fp->end = pnew;
		}
		else
		{
			//β�巨
			fp->end->next = pnew;
			fp->end = pnew;
		}
		//�ڵ����
		(fp->number)++;
	}
	return fp;
}
//��ӡ
//#define a
void show_list(hn* head)
{
#ifdef a
	//ת��Ϊ�ڵ�����
	for (nd* p = head->frist; p != NULL; p = p->next)
	{
		printf("%d  ", p->sum);
	}
	printf("\n");
	printf("������Ľڵ�������Ϊ%d��\n", head->number);
#else
	//
	nd* p = head->frist;
	printf("%d\n", head->number);
	do
	{

		printf("%d \t", p->sum);
		p = p->next;

	} while (p);
#endif

}
//ð������
hn* pao1(hn* head)
{

	for (int i = 0; i < head->number - 1; i++)
	{
		nd* p = head->frist;
		for (int j = 0; j < head->number - i - 1; j++)
		{
			if (p->sum > p->next->sum)
			{
#if 1
				p->sum = (p->sum) ^ (p->next->sum);
				p->next->sum = (p->sum) ^ (p->next->sum);
				p->sum = (p->sum) ^ (p->next->sum);
#else
				int tmp;
				tmp = p->sum;
				p->sum = p->next->sum;
				p->next->sum = tmp;
#endif
			}
			p = p->next;
		}
	}
	return head;
}
//����鲢
hn* add1(hn* head1, hn* head2)
{
	hn* head3 = malloc(sizeof(hn));
	if (NULL == head3)
		return;
	head3->frist = NULL;
	head3->end = NULL;

	nd* p1 = head1->frist;
	nd* p2 = head2->frist;
	head3->number = head1->number + head2->number;
	while (p1 && p2)
	{	
		nd* pnew = malloc(sizeof(nd));
		if (NULL == pnew)
		{
			return;
		}
		pnew->sum = NULL;
		pnew->next = NULL;
		if (p1->sum < p2->sum)
		{
			pnew = p1;
			p1 = p1->next;
			pnew->next = NULL;
		}
		else 
		{
			pnew = p2;
			p2 = p2->next;
			pnew->next = NULL;
		}
		if (head3->frist == NULL)
		{
			head3->end = pnew;
			head3->frist = pnew;
		}
		else
		{
			head3->end->next = pnew;
			head3->end = pnew;
		}
	}
	if (p1 != NULL)
	{
		head3->end->next = p1;
		head3->end = head1->end;
	}
	else
	{
		head3->end->next = p2;
		head3->end = head2->end;
	}
	head1->frist = NULL;
	head1->end = NULL;
	free(head1);

	head2->frist = NULL;
	head2->end = NULL;
	free(head2);
	return head3;
}
void main01()
{
	printf("�������һ������\n");
	hn* head1 = changed_node();
	//printf("����ǰ��\n");
	//show_list(head1);
	pao1(head1);
	//printf("\n");
	//printf("�����\n");
	//show_list(head1);
	printf("\n");

	printf("������ڶ�������\n");
	hn* head2 = changed_node();
	//printf("����ǰ��\n");
	//show_list(head2);
	pao1(head2);
	//printf("\n");
	//printf("�����\n");
	//show_list(head2);
	printf("\n");

	hn* head3 = add1(head1, head2);
	printf("\n");
	printf("�����\n");
	show_list(head3);
	printf("\n");
	printf("%s\n", __FILE__);
	printf("%d\n", __LINE__);
	printf("%s\n", __DATE__);
	printf("%s\n", __TIME__);
	system("pause");
}
//-----------------------------------

#else
typedef int dataType;
//-----------------------------------

// �������ݽڵ�
struct node
{
	dataType data; // ������
	struct node* next; // ָ���򣬴��(ָ��)��һ���ڵ�ĵ�ַ
};
//-----------------------------------

// ����ͷ�ڵ�
struct headNode
{
	struct node* first; // ָ���һ�����ݽڵ�
	struct node* last; // ָ�����һ�����ݽڵ�
	int nodeNumber; // ��¼����ڵ���
};
//---------------------------------
// β��
void tailAdd(struct node* pnew, struct headNode* head)
{
	head->last->next = pnew;
	head->last = pnew;
}
//---------------------------------
// ͷ��
void headAdd(struct node* pnew, struct headNode* head)
{
	pnew->next = head->first;
	head->first = pnew;
}
//---------------------------------
struct headNode* create_list()
{
	// ����ͷ�ڵ�
	struct headNode* head = malloc(sizeof(struct headNode));
	if (head == NULL)
	{
		perror("create headNode failed");
		return NULL;
	}
	head->first = NULL;
	head->last = NULL;
	head->nodeNumber = 0;

	dataType data;

	while (1)
	{
		scanf("%d", &data);
		if (data == 0)// ��������
			break;

		// �����½ڵ�
		struct node* pnew = malloc(sizeof(struct node));
		if (pnew == NULL)
		{
			perror("create newnode failed");
			return NULL;
		}
		pnew->data = data;
		pnew->next = NULL;

		//���½ڵ���뵽����
		if (head->first == NULL)//���޵���
		{
			head->first = pnew;
			head->last = pnew;
		}
		else // ���ٵ���
		{
			// β�巨
			head->last->next = pnew;
			head->last = pnew;

			// ͷ�巨
			//headAdd(pnew,head);

		}

		// ���½ڵ���
		head->nodeNumber++;
	}

	return head;
}
//------------------------------
// ��ӡ�����
void show_list(struct headNode* head)
{
	for (struct node* p = head->first; p != NULL; p = p->next)
	{
		printf("%d  ", p->data);
	}
	printf("\n");
	printf("������Ľڵ�������Ϊ%d��\n", head->nodeNumber);
}


int main()
{
	// ����������
	struct headNode* head = create_list();

	//��ӡ����
	show_list(head);
	system("pause");
	return 0;
}
#endif
#endif