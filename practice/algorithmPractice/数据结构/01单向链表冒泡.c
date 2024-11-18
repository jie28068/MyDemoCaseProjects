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
//定义节点
typedef struct node
{
	int sum;
	struct node* next;
}nd;
//定义头节点结构体
typedef struct HeadNode
{
	nd* frist;
	nd* end;
	int number;
}hn;

//创建节点
hn* changed_node()
{
	int a = 0;
	//创建头结点
	hn* fp = malloc(sizeof(hn));
	if (fp == NULL)
		return;
	fp->frist = NULL;
	fp->end = NULL;
	fp->number = 0;

	while (1)
	{
		//创建新的节点
		nd* pnew = malloc(sizeof(nd));
		nd* temp = malloc(sizeof(nd));
		if (pnew == NULL)
			return;
		//输入数据
		printf("请输入数据:\n");
		scanf("%d", &a);
		if (-1 == a)
			break;   /*人麻了*/
		//给节点赋值
		pnew->sum = a;
		pnew->next = NULL;
		//判断是否为为第一个节点
		if (NULL == fp->frist)
		{
			fp->frist = pnew;
			fp->end = pnew;
		}
		else
		{
			//尾插法
			fp->end->next = pnew;
			fp->end = pnew;
		}
		//节点计数
		(fp->number)++;
	}
	return fp;
}
//打印
//#define a
void show_list(hn* head)
{
#ifdef a
	//转换为节点类型
	for (nd* p = head->frist; p != NULL; p = p->next)
	{
		printf("%d  ", p->sum);
	}
	printf("\n");
	printf("此链表的节点数据数为%d个\n", head->number);
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
//冒泡排序
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
//链表归并
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
	printf("请输入第一条链表：\n");
	hn* head1 = changed_node();
	//printf("排序前：\n");
	//show_list(head1);
	pao1(head1);
	//printf("\n");
	//printf("排序后：\n");
	//show_list(head1);
	printf("\n");

	printf("请输入第二条链表：\n");
	hn* head2 = changed_node();
	//printf("排序前：\n");
	//show_list(head2);
	pao1(head2);
	//printf("\n");
	//printf("排序后：\n");
	//show_list(head2);
	printf("\n");

	hn* head3 = add1(head1, head2);
	printf("\n");
	printf("重组后：\n");
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

// 定义数据节点
struct node
{
	dataType data; // 数据域
	struct node* next; // 指针域，存放(指向)下一个节点的地址
};
//-----------------------------------

// 定义头节点
struct headNode
{
	struct node* first; // 指向第一个数据节点
	struct node* last; // 指向最后一个数据节点
	int nodeNumber; // 记录链表节点数
};
//---------------------------------
// 尾插
void tailAdd(struct node* pnew, struct headNode* head)
{
	head->last->next = pnew;
	head->last = pnew;
}
//---------------------------------
// 头插
void headAdd(struct node* pnew, struct headNode* head)
{
	pnew->next = head->first;
	head->first = pnew;
}
//---------------------------------
struct headNode* create_list()
{
	// 创建头节点
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
		if (data == 0)// 链表生成
			break;

		// 创建新节点
		struct node* pnew = malloc(sizeof(struct node));
		if (pnew == NULL)
		{
			perror("create newnode failed");
			return NULL;
		}
		pnew->data = data;
		pnew->next = NULL;

		//将新节点插入到链表
		if (head->first == NULL)//从无到有
		{
			head->first = pnew;
			head->last = pnew;
		}
		else // 从少到多
		{
			// 尾插法
			head->last->next = pnew;
			head->last = pnew;

			// 头插法
			//headAdd(pnew,head);

		}

		// 更新节点数
		head->nodeNumber++;
	}

	return head;
}
//------------------------------
// 打印链表节
void show_list(struct headNode* head)
{
	for (struct node* p = head->first; p != NULL; p = p->next)
	{
		printf("%d  ", p->data);
	}
	printf("\n");
	printf("此链表的节点数据数为%d个\n", head->nodeNumber);
}


int main()
{
	// 创建新链表
	struct headNode* head = create_list();

	//打印链表
	show_list(head);
	system("pause");
	return 0;
}
#endif
#endif