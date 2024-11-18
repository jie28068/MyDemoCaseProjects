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
   * Explain:倒数第几个就是正数的第N-n+1个，用它作为为循环条件，找到要删除的节点
   *
   * 也可以用快慢指针（相隔n）和栈实现（弹出第n个）
 */
 //栈
struct Stack {
	struct ListNode* val;
	struct Stack* next;
};
struct ListNode {
	int val;
	struct ListNode* next;
};
//生成链表
struct ListNode* make7()
{
	//定义哑节点
	struct ListNode* dummy = malloc(sizeof(struct ListNode));
	if (NULL == dummy)
	{
		return;
	}
	dummy->next = NULL;
	dummy->val = 0;
	//定义一个遍历指针
	struct ListNode* p = dummy;
	printf("请输入节点：\n");
	while (1)
	{
		struct ListNode* pnew = malloc(sizeof(struct ListNode));
		if (NULL == pnew)
		{
			return;
		}
		int a = 0;
		scanf("%d", &a);
		if (0 == a)
		{
			break;
		}
		pnew->next = NULL;
		pnew->val = a;
		//尾插
		p->next = pnew;
		p = pnew;
	}
	dummy = dummy->next;
	return dummy;
}
//实现
struct ListNode* removeNthFromEnd7(struct ListNode* head, int n)
{
	//定义遍历指针
	struct ListNode* p = head;
	struct ListNode* pp = head;
	struct ListNode* pre = head;
	//得到链表长度
	int N = 0;
	while (pp)
	{
		pp = pp->next;
		N++;
	}
	//找到要删除的点
	int sum = N - n + 1;
	while (--sum)
	{
		pre = p;
		p = p->next;
	}

	//删除节点
	if (pre == head && p == head)//为第一个时
	{
		head = head->next;
	}
	else
	{
		pre->next = NULL;
		pre->next = p->next;
		free(p);
	}

	//返回
	return head;
}
//输出节点
void put7(struct ListNode* p)
{
	while (p)
	{
		printf("%d\t ", p->val);
		p = p->next;
	}
}
int main7()
{
	struct ListNode* head = make7();
	int n = 0;
	printf("请输入要删除的节点\n");
	scanf("%d", &n);
	head = removeNthFromEnd7(head, n);
	put7(head);
	system("pause");
	return 0;
}
#endif