#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */

typedef struct listnode
{
	int val;
	listnode* next;
	listnode(int x) :val(x), next(NULL) {}
}Listnode;

class Solution
{
public:
	Listnode* func(Listnode* head)
	{
		Listnode pnode(0);
		pnode.next = head;
		listnode* p = &pnode;
		while (p)
		{
			int sum = 0;
			listnode* q = p->next;
			while (q)
			{
				sum += q->val;
				if (sum == 0)
				{
					p->next = q->next;
				}
				q = q->next;
			}
			p = p->next;
		}
		return pnode.next;
	}
};
int main()
{
	Listnode a(1);
	Listnode b(2);
	Listnode c(-3);
	Listnode d(3);
	Listnode r(1);
	Solution s;
	a.next = &b;
	b.next = &c;
	c.next = &d;
	d.next = &r;
	Listnode* p = s.func(&a);
	while (p)
	{
		cout << p->val << endl;
		p = p->next;
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
