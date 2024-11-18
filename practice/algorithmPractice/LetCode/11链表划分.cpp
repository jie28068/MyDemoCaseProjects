#if 0
#include <iostream>
using namespace std;

/*给定一个数字，链表中大于此数的放前面，小于的放后面，但是不改变原有的数据链式*/
//结构体
typedef struct s_list
{
	//数据域
	int data;
	//指针域
	s_list* next;
	//构造函数
	s_list(int x) :data(x), next(NULL) {}
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

	//链表划分
	l_list* h_f(l_list* p);
};

//生成链表
l_list* List::make_list()
{
	l_list* head = new l_list(0);
	head->next = NULL;
	l_list* p = head;
	while (1)
	{
		int a = 0;
		cin >> a;
		if (a == 0)
			break;
		l_list* node = new l_list(a);
		//尾插
		node->next = NULL;

		p->next = node;
		p = node;

	}
	return head->next;
}

//打印链表
void List::pr_list(l_list* p)
{
	while (p)
	{
		cout << "****************" << endl;
		cout <<p->data << endl;
		p = p->next;
	}
}

//划分链表
l_list* List::h_f(l_list* p)
{
	//定义俩条链表
	l_list* p1 = new l_list(0);
	p1->next = NULL;
	l_list* pp1 = p1;
	l_list* p2 = new l_list(0);
	p2->next = NULL;
	l_list* pp2 = p2;
	int x = 3;
	//循环
	while (p)
	{
		if (p->data < x)
		{
			p1->next = p;
			p1 = p;
		}
		else
		{
			p2->next = p;
			p2 = p;
		}
		p = p->next;
	}
	p1->next = pp2->next;
	p2->next = NULL;
	return pp1->next;
}

int main()
{
	List l;
	l.pr_list(l.h_f(l.make_list()));
	system("pause");
	return EXIT_SUCCESS;
}

#endif