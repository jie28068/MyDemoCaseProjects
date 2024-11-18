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
	////构造函数
	//s_list(int x) :data(x), next(NULL) {}
}l_list;

class list
{
public:
	//生成链表
	l_list* make()
	{
		cout << "请输入一串数字，按0结束" << endl;
		//定义一个哑节点
		l_list* head = new l_list;
		head->next = NULL;
		//定义一个遍历指针
		l_list* p = NULL;
		p = head;
		while (1)
		{
			int a = 0;
			cin >> a;
			if (a == 0)
				break;
			l_list* node = new l_list;

			//如果大于零尾插
			if (a > 0)
			{
				//尾插
				node->next = NULL;
				node->data = a;
				p->next = node;
				p = node;
			}
			else if(a < 0)
			{
				//头插
				node->data = a;				
				node->next = head->next;
				head->next = node;			
			}

		}
		return head->next;
	}

	//打印链表
	void pr_list(l_list* p)
	{
		
		while (p)
		{
			cout << "****************" << endl;
			cout << p->data << endl;
			p = p->next;
		}
	}
};
int main()
{
	list l;
	l.pr_list(l.make());
	system("pause");
	return  EXIT_SUCCESS;
}
#endif;
