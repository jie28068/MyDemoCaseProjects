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
	listnode(int x):val(x),next(NULL){}
}Listnode;

class Solution
{
public:
	Listnode* func(Listnode* head)
	{
		if (head == nullptr)
		{

			return NULL;
		}
		Listnode* per = head;
		Listnode* pnext = head->next;

		if (head->val == head->next->val)
		{
			return head->next;
		}

		while (pnext)
		{
			if (per->val == pnext->val)
			{
				while (per->val == pnext->val)
				{
					pnext = pnext->next;
				}
				per->next = pnext;
			}
			per = per->next;
			pnext = pnext->next;
		}
		return head;
	}
};
int main()
{
	Listnode a(1);
	Listnode b(2);
	Listnode c(3);
	Listnode d(3);
	Listnode r(3);
	Listnode e(4);
	Listnode f(4);
	Listnode g(5);
	Solution s;
	a.next = &b;
	b.next = &c;
	c.next = &d;
	d.next = &r;
	r.next = &e;
	e.next = &f;
	f.next = &g;
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
