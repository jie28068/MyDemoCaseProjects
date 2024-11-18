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
	l_list* ni_list(l_list* head ,int m ,int n);
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
		int a = 0;
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
l_list* List::ni_list(l_list* p ,int m ,int n)
{
	if (p == NULL)
	{
		return;
	}
	int len = n - m + 1; //计算要逆置的长度
	l_list* head = p;    //记录头结点
	l_list* pre = NULL;  //要逆置的前驱节点
	while (p && (--m))
	{
		 pre = p;
		p = p->next;
	}
	l_list* temp = p; //记录逆置头节点
	l_list* newp = NULL; //定义暂时存放指针
	while (--len)
	{
		l_list* next = p->next; //定义遍历指针
		p->next = newp;     //每拿出一个就指向前一个
		newp = p;
		p = next;
	}
	temp->next = p;
	if (pre)
	{
		pre->next = newp;
	}
	else          //如果逆置的是第一个节点
	{
		head = newp;
	}
	return head;
}

int main()
{
	List l;
	int n, m;
	cin >> m >> n;
	l.pr_list(l.ni_list(l.make_list() ,m ,n));
	system("pause");
	return  EXIT_SUCCESS;
}
#endif;
