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
//����һ���ṹ��
struct node
{
	int data; //������
	struct node* next;//ָ����
};
//�����ڵ�
struct node* inster()
{
//�����ƽڵ�
	struct node* dummy = malloc(sizeof(struct node));
	if (NULL == dummy)
		return;
	dummy->next = NULL;
	dummy->data = 0;
	//����һ������ָ��
	printf("����ڵ�\n");
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
		//β��
		p->next = pnew;
		p = pnew;
	}
	dummy = dummy->next;
	return dummy;
}
//����ڵ�
void put(struct node * p)
{
	while (p)
	{
		printf("%d\t ", p->data);
		p = p->next;
	}
}
//��������ó�����
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
//����һ����������  T=O(L) ,O(1)
struct node* removeNthFromEnd(struct node* head, int lo,int n)
{
	//��һ��ѽָ��
	struct node* pya = malloc(sizeof(struct node));
	//
	if (NULL == pya)
		return;
	pya->next = head;
	pya->data = 0;
	//����һ������ָ��
	struct node* ptm = pya;
	//����ѭ����ָ����λ��
	for (int i = 1; i < lo - n + 1; i++)
	{
		ptm = ptm->next;
	}
	//ɾ���ڵ�
	ptm->next = ptm->next->next;
	struct node* hea = pya->next;
	free(pya);
	return hea;
}
//������������ָ��  T=O(L),O(1)
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
//��������ջ    T=O(L),O=O(L)(��Ҫ��ջ�Ŀռ俪��)
//����ջ
struct stack3
{
	//ջ��ָ��
	struct stack3* next;
	//�ڵ�ָ��
	struct node* val;
};
struct node* stacklist(struct node* head, int lo, int n)
{
	//�ƽڵ�
	struct node* dummy3 = malloc(sizeof(struct node));
	dummy3->data = 0;
	dummy3->next = head;
	//ջ��ָ���ʼ��
	struct stack3* stk = NULL;
	//����һ��������
	struct node* cur = dummy3;
	//��ջ
	while (cur)
	{
		//һ������ֵ��ָ��
		struct stack3 *temp= malloc(sizeof(struct node));
		//�õ���ǰ�ڵ�ŵ�ջ��
		temp->val = cur;
		//Ϊ��ʹջ��ָ������ָ
		temp->next = stk;
		//����ջ����ջ��һֱ����dummy3��0��
		stk = temp;
		cur = cur->next;
	}
	//��ջ
	for (int i = 0; i < n; ++i)
	{
		struct stack3* temp = stk->next;
		//Фջ
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
	printf("Ҫɾ���Ľڵ�\n");
	scanf("%d", &n);
	//p = removeNthFromEnd(p, lo ,n);
	//p = twop(p, lo, n);
	p= stacklist(p, lo, n);
	put(p);
	system("pause");
	return 0;
}
#endif