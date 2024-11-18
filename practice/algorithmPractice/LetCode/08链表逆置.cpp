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

struct listnode
{
	int data;
	listnode* next;
	listnode(int x):data(x),next(NULL) {}
};

listnode* func(listnode* head)
{
	listnode* begin = head;
	listnode* newhead = NULL;

	while (head)
	{
		listnode* next = head->next;
		head->next = newhead;
		newhead = head;
		head = next;
	}
	
	return newhead;
}

int main()
{
	listnode a(0);
	listnode b(1);
	listnode c(2);
	listnode d(3);
	listnode e(4);
	a.next = &b;
	b.next = &c;
	c.next = &d;
	d.next = &e;
	listnode* p = func(&a);

	while (p)
	{
		cout << p->data << endl;
		p = p->next;
	}

	system("pause");
	return  EXIT_SUCCESS;
}
#endif
