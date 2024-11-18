#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include <vector>
#include <algorithm>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
   * 方法一：暴力合并
   * 方法二：使用容器拆解后排序合并
   * 方法三：俩个俩个逐一合并  容器拆成俩个子容器，递归调用为俩条链表的函数
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

extern bool func_list(const l_list* , const l_list* );

class list
{
public:
	l_list* h_list(vector<l_list*>& list)
	{
		vector<l_list*> v_node;
		for (int i = 0; i < list.size(); i++)
		{
			l_list* head = list[i];

			while (head)
			{
				//放入另一个容器内
				v_node.push_back(head);
				head = head->next;
			}
		}

		//为空时
		if (v_node.size() == 0)
		{
			return NULL;
		}

		//排序
		sort(v_node.begin(), v_node.end(), func_list);

		//链接
		for (int i = 1; i < v_node.size(); i++)
		{
			v_node[i - 1]->next = v_node[i];
		}
		v_node[v_node.size() - 1]->next = NULL;

		return v_node[0];
	}

};

bool func_list(const l_list* a, const l_list* b)
{
	return a->data < b->data;
}
int main()
{
	std::vector<l_list*> v_node;
	l_list a(1);
	l_list b(4);
	l_list c(6);
	l_list d(0);
	l_list e(5);
	l_list f(7);
	l_list g(2);
	l_list h(3);

	a.next = &b;
	b.next = &c;
	d.next = &e;
	e.next = &f;
	g.next = &h;

	list l;
	v_node.push_back(&a);
	v_node.push_back(&d);
	v_node.push_back(&g);

	l_list* p = l.h_list(v_node);

	while (p)
	{
		cout << p->data <<endl;
		p = p->next;
	}

	system("pause");
	return  EXIT_SUCCESS;
}
#endif;
