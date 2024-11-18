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
//定义一个结构体
struct node
{
	int data; //数据域
	struct node* next;//指针域
};
//创建节点
struct node* inster()
{
//定义哑节点
	struct node* dummy = malloc(sizeof(struct node));
	if (NULL == dummy)
		return;
	dummy->next = NULL;
	dummy->data = 0;
	//定义一个遍历指针
	printf("输入节点\n");
	struct node* p = dummy;
	while (1)
	{
		struct node* pnew = malloc(sizeof(struct node));
		if (NULL == pnew)
		{
			return;
		}
		int a = 0;
		scanf("%d", &a);
		if (0 == a)
			break;
		pnew->next = NULL;
		pnew->data = a;
		//尾插
		p->next = pnew;
		p = pnew;
	}
	dummy = dummy->next;
	return dummy;
}
//输出节点
void put(struct node * p)
{
	while (p)
	{
		printf("%d\t ", p->data);
		p = p->next;
	}
}
//遍历链表得出长度
int height(struct node* p)
{
	int h = 0;
	while (p)
	{
		h++;
		p = p->next;
	}
	return h;
}
//方法一，遍历俩遍  T=O(L) ,O(1)
struct node* removeNthFromEnd(struct node* head, int lo,int n)
{
	//定一个呀指针
	struct node* pya = malloc(sizeof(struct node));
	//
	if (NULL == pya)
		return;
	pya->next = head;
	pya->data = 0;
	//定义一个遍历指针
	struct node* ptm = pya;
	//利用循环到指定的位置
	for (int i = 1; i < lo - n + 1; i++)
	{
		ptm = ptm->next;
	}
	//删除节点
	ptm->next = ptm->next->next;
	struct node* hea = pya->next;
	free(pya);
	return hea;
}
//方法二，快慢指针  T=O(L),O(1)
struct node* twop(struct node* head, int lo, int n)
{
	struct node* dummy1 = malloc(sizeof(struct node));
	dummy1->data = 0, dummy1->next = head;
	struct node* first = head;
	struct node* second = dummy1;
	for (int i = 0; i < n; ++i) {
		first = first->next;
	}
	while (first) {
		first = first->next;
		second = second->next;
	}
	second->next = second->next->next;
	struct node* ans = dummy1->next;
	free(dummy1);
	return ans;
}
//方法三，栈    T=O(L),O=O(L)(主要是栈的空间开销)
//定义栈
struct stack3
{
	//栈顶指针
	struct stack3* next;
	//节点指针
	struct node* val;
};
struct node* stacklist(struct node* head, int lo, int n)
{
	//哑节点
	struct node* dummy3 = malloc(sizeof(struct node));
	dummy3->data = 0;
	dummy3->next = head;
	//栈顶指针初始化
	struct stack3* stk = NULL;
	//定义一个遍历节
	struct node* cur = dummy3;
	//入栈
	while (cur)
	{
		//一个交换值的指针
		struct stack3 *temp= malloc(sizeof(struct node));
		//拿到当前节点放到栈中
		temp->val = cur;
		//为了使栈内指针向下指
		temp->next = stk;
		//更新栈顶，栈顶一直会是dummy3（0）
		stk = temp;
		cur = cur->next;
	}
	//出栈
	for (int i = 0; i < n; ++i)
	{
		struct stack3* temp = stk->next;
		//肖栈
		free(stk);
		stk = temp;
	}
	struct node* prv = stk->val;
	prv->next = prv->next->next;
	struct node* aus = dummy3->next;
	return aus;
}
int main()
{
	struct node* p = inster();
	int lo = height(p);
	int n = 0;
	printf("要删除的节点\n");
	scanf("%d", &n);
	//p = removeNthFromEnd(p, lo ,n);
	//p = twop(p, lo, n);
	p= stacklist(p, lo, n);
	put(p);
	system("pause");
	return 0;
}
#endif