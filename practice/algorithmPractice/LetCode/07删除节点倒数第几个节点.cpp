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
   * Explain:�����ڼ������������ĵ�N-n+1����������ΪΪѭ���������ҵ�Ҫɾ���Ľڵ�
   *
   * Ҳ�����ÿ���ָ�루���n����ջʵ�֣�������n����
 */
 //ջ
struct Stack {
	struct ListNode* val;
	struct Stack* next;
};
struct ListNode {
	int val;
	struct ListNode* next;
};
//��������
struct ListNode* make7()
{
	//�����ƽڵ�
	struct ListNode* dummy = malloc(sizeof(struct ListNode));
	if (NULL == dummy)
	{
		return;
	}
	dummy->next = NULL;
	dummy->val = 0;
	//����һ������ָ��
	struct ListNode* p = dummy;
	printf("������ڵ㣺\n");
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
		//β��
		p->next = pnew;
		p = pnew;
	}
	dummy = dummy->next;
	return dummy;
}
//ʵ��
struct ListNode* removeNthFromEnd7(struct ListNode* head, int n)
{
	//�������ָ��
	struct ListNode* p = head;
	struct ListNode* pp = head;
	struct ListNode* pre = head;
	//�õ�������
	int N = 0;
	while (pp)
	{
		pp = pp->next;
		N++;
	}
	//�ҵ�Ҫɾ���ĵ�
	int sum = N - n + 1;
	while (--sum)
	{
		pre = p;
		p = p->next;
	}

	//ɾ���ڵ�
	if (pre == head && p == head)//Ϊ��һ��ʱ
	{
		head = head->next;
	}
	else
	{
		pre->next = NULL;
		pre->next = p->next;
		free(p);
	}

	//����
	return head;
}
//����ڵ�
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
	printf("������Ҫɾ���Ľڵ�\n");
	scanf("%d", &n);
	head = removeNthFromEnd7(head, n);
	put7(head);
	system("pause");
	return 0;
}
#endif