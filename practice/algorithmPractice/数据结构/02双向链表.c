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
//结构体节点
struct node
{
	struct node* ppev;
	int data;
	struct node* pnext;
};
//结构体头结点
struct headnode
{
	struct node* first;
	struct node* last;
	int number;
};
//生成节点
struct headnode* make2()
{
	//生成头结点
	struct headnode* head = malloc(sizeof(struct headnode));
	if (NULL == head)
		return;
	//初始化头结点
	head->first = NULL;
	head->last = NULL;
	head->number = 0;
	//输入数据
	while (1)
	{
		int a = 0;
		scanf("%d", &a);
		if (0 == a)
		{
			break;
		}
		//定义新节点
		struct node* pnew = malloc(sizeof(struct node));
		if (NULL == pnew)
			return;
		//给节点赋值
		pnew->data = a;
		pnew->ppev = NULL;
		pnew->pnext = NULL;
		//链接到链表上
		if (head->first == NULL) //开始为空
		{
			head->first = pnew;
			head->last = pnew;
		}
		else//开始不为空，尾插
		{
			head->last->pnext = pnew;
			pnew->ppev = head->last;
			head->last = pnew;
		}
		head->number++;
	}
	return head;
}
//打印
void show_list(struct headnode* head)
{
	//转换为节点类型
	for (struct node* p = head->first; p != NULL; p = p->pnext)
	{
		printf("%d  ", p->data);
	}
	printf("\n");
	printf("此链表的节点数据数为%d个\n", head->number);
}
//删除节点
struct headnode* delete2(struct headnode *head ,int len)
{
	struct node* pw = malloc(sizeof(struct node));
	if (NULL == pw)
		return;
	pw = head->first;
	//先遍历找到要删除的值 
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
	if (head->first == pw) //如果是第一个节点
	{
		head->first = pw->pnext;
		pw->pnext->ppev = NULL;
		pw->pnext = NULL;
		head->number--;
	}
	else if(head->last == pw)//如果是尾结点
	{
		head->last = pw->ppev;
		pw->ppev->pnext = NULL;
		pw->ppev = NULL;
		head->number--;
	}
	else if (pw == NULL)//找不到删除的值时
	{
		printf("找不到！\n");
	}
	else//如果是中间的值
	{
		pw->pnext->ppev = pw->ppev;
		pw->ppev->pnext = pw->pnext;
		head->number--;
	}
	return head;
}
//判断当前链表是否有这个值
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
//找出俩条链表中相同的值
struct headnode* compare(struct headnode* head1, struct headnode* head2)
{
	//产生一个新的链表
	struct headnode* head3 = malloc(sizeof(struct headnode));
	if (NULL == head3)
		return;
	//初始化链表
	head3->first = NULL;
	head3->last = NULL;
	head3->number = 0;
	//循环遍历节点
	for (struct node* p1 = head1->first;p1!=NULL;p1=p1->pnext)
	{
		for (struct node* p2 = head2->first; p2 != NULL; p2 = p2->pnext)
		{		
			if (p1->data == p2->data)
			{   
				//设置一个临时节点存放p2
				struct node* ptemp = malloc(sizeof(struct node));
				if (NULL == ptemp)
					return;
				//初始化定义
				ptemp->data = p2->data;
				ptemp->pnext = NULL;
				ptemp->ppev = NULL;
				//判断是否已经有这个值
				int a = pan(ptemp->data, head3);
				//为空时插入头结点
				if (head3->first == NULL)
				{
					head3->first = ptemp;
					head3->last = ptemp;
					head3->number++;
				}
				//不为空时
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
//释放堆空间
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
