#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <set>
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
	s_list(int x) :data(x), next(NULL) { }
}l_list;

class list
{
public:

	//set容器
	void list_set(l_list* p1 ,l_list *p2)
	{
		std::set<l_list*> test;

		//插入set容器中
		while(p2)
		{
			test.insert(p2);
			p2 = p2->next;
		}
		
		//在容器中遍历查找相同的值
		while (p1)
		{
			if (test.find(p1) != test.end())
			{
				cout << p1->data << endl;
				break;
			}		
			p1 = p1->next;
		}
	}

	//计算链表长度
	int len_list(l_list* p)
	{
		int len = 0;
		while (p)
		{
			p = p->next;
			len++;
		}
		return len;
	}

	//普通方式
	void list_h(l_list* p1, l_list* p2)
	{
		int len1 = len_list(p1);
		int len2 = len_list(p2);

		if (len1 > len2)
		{
			int c_len = len1 - len2;
			while (c_len)
			{
				p1 = p1->next;
				c_len--;
			}
		}
		else if(len1 < len2)
		{
			int c_len = len2 - len1;
			while (c_len)
			{
				p2 = p2->next;
				c_len--;
			}
		}

		while (p1 && p2)
		{
			if (p1 == p2)
			{
				cout << p1->data << endl;
				break;
			}
			p1 = p1->next;
			p2 = p2->next;
		}
	}
};


int main()
{
	list l;
	l_list* a = new l_list(2);
	l_list* b = new l_list(3);
	l_list* c = new l_list(4);
	l_list* d = new l_list(5);
	l_list* e = new l_list(6);
	l_list* f = new l_list(7);
	l_list* g = new l_list(8);
	l_list* k = new l_list(10);
	a->next = b;
	b->next = c;
	c->next = f;
	d->next = e;
	e->next = f;
	f->next = g;
	f->next = k;

	//l.list_set(a, d);
	
	l.list_h(a, d);


	system("pause");
	return  EXIT_SUCCESS;
}
#endif;
