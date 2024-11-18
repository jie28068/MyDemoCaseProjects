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
typedef struct s_list
{
	//数据域
	int data;
	//指针域
	s_list* next;
	//构造函数
	s_list(int x) :data(x), next(NULL) {}
}l_list;

class list
{
public:
	list()
	{
		cout << "构造成功" << endl;
	}
	l_list* bin_list(l_list* l1, l_list* l2)
	{
		l_list h(0);
		l_list* head = &h;
	
		while (l1 && l2)
		{
			if (l1->data < l2->data)
			{
				head->next = l1;
				l1 = l1->next;
			}
			else
			{
				head->next = l2;
				l2 = l2->next;
			}
			head = head->next;
		}
		if (l1)
		{
			head->next = l1;
		}
		if (l2)
		{
			head->next = l2;
		}
		return h.next;
	}

private:

};

int main()
{
	l_list a(1);
	l_list b(4);
	l_list c(6);
	l_list d(0);
	l_list e(5);
	l_list f(7);

	a.next = &b;
	b.next = &c;
	d.next = &e;
	e.next = &f;

	list l;
	l_list* p = l.bin_list(&a, &d);
	while (p)
	{
		cout << "链接后的链表：" << p->data << endl;
		p = p->next;
	}

	system("pause");
	return  EXIT_SUCCESS;
}
#endif;
