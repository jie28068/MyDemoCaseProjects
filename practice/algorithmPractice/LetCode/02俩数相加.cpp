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
   * Explain:输入俩组数字，分别存在俩串链表中，然后求和
 */
struct ListNode {
	int val; //数据域
	struct ListNode* next;//指针域
};
//实现函数，传递俩条链表进来
struct ListNode* addNode(struct ListNode* l1, struct ListNode* l2)
{
	//定义链表
	struct ListNode* head = NULL;
	struct ListNode* tail = NULL;
	//定义进制位
	int carry = 0;
	//遍历俩条链表
	while (l1 || l2)
	{
		//将链表中的值取出，没有数字了就赋值为0
		int n1 = l1 ? l1->val : 0;
		int n2 = l2 ? l2->val : 0;
		//俩数和
		int sum = n1 + n2 + carry;
		//判断首节点是否为空
		if (!head)
		{	//为空的情况下
			head = tail = malloc(sizeof(struct ListNode));
			tail->val = sum % 10;
			tail->next = NULL;
		}
		else
		{
			tail->next = malloc(sizeof(struct ListNode));//创建下一个节点的堆空间
			tail->next->val = sum % 10;
			tail = tail->next;
			tail->next = NULL;
		}
		carry = sum / 10;
		if (l1)
		{
			l1 = l1->next;
		}
		if (l2)
			l2 = l2->next;
	}
	if (carry > 0)
	{
		tail->next = malloc(sizeof(struct ListNode));
		tail->next->val = carry;
		tail->next->next = NULL;
	}
	return head;
}
//生成链表
struct ListNode* make2()
{
	printf("亲输入链表：\n");
	//定义一个首节点
	struct ListNode* head = malloc(sizeof(struct ListNode));
	if (NULL == head)
		return;
	/**/
	head->next = NULL;//哑节点
	head->val = 0;   //默认为零
	struct ListNode* pp = NULL;
	pp = head;
	while (1)
	{
		int a = 0;
		scanf("%d", &a);
		if (0 == a)
			break;
		struct ListNode* pnew = malloc(sizeof(struct ListNode));
		if (NULL == pnew)
		{
			return;
		}
		pnew->val = a;
		pnew->next = NULL;
		//
		pp->next = pnew;
		pp = pnew;
	}
	head = head->next;
	return head;
}
// 打印链表节
void show_list(struct ListNode* l1)
{
	while (l1)
	{
		printf("%d \t", l1->val);
		l1 = l1->next;
	}
}
int main()
{
	struct ListNode* l1 = make2();
	struct ListNode* l2 = make2();
	struct ListNode* l3 = addNode(l1, l2);
	show_list(l3);
	system("pause");
	return 0;
}
#endif