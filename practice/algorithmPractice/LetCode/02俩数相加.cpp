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
   * Explain:�����������֣��ֱ�������������У�Ȼ�����
 */
struct ListNode {
	int val; //������
	struct ListNode* next;//ָ����
};
//ʵ�ֺ��������������������
struct ListNode* addNode(struct ListNode* l1, struct ListNode* l2)
{
	//��������
	struct ListNode* head = NULL;
	struct ListNode* tail = NULL;
	//�������λ
	int carry = 0;
	//������������
	while (l1 || l2)
	{
		//�������е�ֵȡ����û�������˾͸�ֵΪ0
		int n1 = l1 ? l1->val : 0;
		int n2 = l2 ? l2->val : 0;
		//������
		int sum = n1 + n2 + carry;
		//�ж��׽ڵ��Ƿ�Ϊ��
		if (!head)
		{	//Ϊ�յ������
			head = tail = malloc(sizeof(struct ListNode));
			tail->val = sum % 10;
			tail->next = NULL;
		}
		else
		{
			tail->next = malloc(sizeof(struct ListNode));//������һ���ڵ�Ķѿռ�
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
//��������
struct ListNode* make2()
{
	printf("����������\n");
	//����һ���׽ڵ�
	struct ListNode* head = malloc(sizeof(struct ListNode));
	if (NULL == head)
		return;
	/**/
	head->next = NULL;//�ƽڵ�
	head->val = 0;   //Ĭ��Ϊ��
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
// ��ӡ�����
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