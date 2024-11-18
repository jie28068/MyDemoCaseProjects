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

//结构体
typedef struct Lis
{
	//指针域
	struct Lis* next;
	//数据域
	int date;
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
		
	//链表逆置
	l_list* ni_list(l_list* head);
};

//生成链表
l_list* List::make_list()
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
		int a=0;
		cin >> a;
		if (a == 0)
			break;
		l_list* node = new l_list;
		//尾插
		node->next = NULL;
		node->date = a;
		
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
		cout << p->date << endl;
		p = p->next;
	}
}

//链表逆置
l_list* List::ni_list(l_list* head)
{
	//思想：逐个反转，定义俩个指针一个指向头，一个用来遍历
	l_list* new_head = NULL;	
	while (head)
	{
		l_list* n = head->next;  //遍历指针
		head->next = new_head;	
		new_head = head;
		head = n;	
	}
	return new_head;
}


int main()
{
	List l;
	//l.pr_list(l.make_list());
	l.pr_list(l.ni_list(l.make_list()));
	system("pause");
	return  EXIT_SUCCESS;
}
#endif;