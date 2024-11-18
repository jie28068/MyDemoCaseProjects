#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <set>     //方法一： 使用set容器
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
//结构体
typedef struct s_list
{
	//数据域
	int data;
	//指针域
	 s_list* next;
	//构造函数
	s_list(int x):data(x),next(NULL){}
}l_list;

//链表类
class List
{
public:
	//构造中
	List()
	{
		cout << "构造中..." << endl;
	}

	//生成链表
	l_list* make_list();

	//打印链表
	void pr_list(l_list* p);

	//方法一，set容器
	void list_set(l_list* p);

	//方法二，快慢指针
	void two_p(l_list* p);
};

//生成环状链表
l_list* List::make_list()
{
	l_list* head = new l_list(1);
	l_list* a = new l_list(2);
	l_list* b = new l_list(3);
	l_list* c = new l_list(4);
	l_list* d = new l_list(5);
	l_list* e = new l_list(6);
	l_list* f = new l_list(7);
	head->next = a;
	a->next = b;
	b->next = c;
	c->next = d;
	d->next = e;
	e->next = f;
	f->next = b;
	return head;
}

//打印链表
void List::pr_list(l_list* p)
{
	while (p)
	{
		cout << "****************" << endl;
		cout << p->data << endl;
		p = p->next;
	}
}

//set
void List::list_set(l_list* p)
{
	std::set<l_list*> test;
	while (p)
	{
		if (test.find(p) != test.end())
		{
			cout<<p->data<<endl;
			break;
		}
		test.insert(p);
		p = p->next;
	}
}

//快慢指针
void List::two_p(l_list* p)
{
	l_list* fast = p;
	l_list* slow = p;
	l_list* meet = NULL;
	while (fast)
	{
		//快慢指针各走一步
		fast = fast->next;
		slow = slow->next;
		if (!fast)
		{
			break;
		}
		fast = fast->next; //快指针再走一步
		if (fast == slow)//
		{
			meet = fast;
			break;
		}	
	}
	if (meet == NULL)
	{
		cout << "没有相遇" << endl;
	}
	while (meet&&p)
	{
		if (p == meet)
		{
			cout << p->data << endl;
		}
		meet = meet->next;
		p = p->next;
	}
}

int main()
{
	List l;
	//l.list_set(l.make_list());
	l.two_p(l.make_list());
	system("pause");
	return  EXIT_SUCCESS;
}

#endif;


