#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <set>     //����һ�� ʹ��set����
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
//�ṹ��
typedef struct s_list
{
	//������
	int data;
	//ָ����
	 s_list* next;
	//���캯��
	s_list(int x):data(x),next(NULL){}
}l_list;

//������
class List
{
public:
	//������
	List()
	{
		cout << "������..." << endl;
	}

	//��������
	l_list* make_list();

	//��ӡ����
	void pr_list(l_list* p);

	//����һ��set����
	void list_set(l_list* p);

	//������������ָ��
	void two_p(l_list* p);
};

//���ɻ�״����
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

//��ӡ����
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

//����ָ��
void List::two_p(l_list* p)
{
	l_list* fast = p;
	l_list* slow = p;
	l_list* meet = NULL;
	while (fast)
	{
		//����ָ�����һ��
		fast = fast->next;
		slow = slow->next;
		if (!fast)
		{
			break;
		}
		fast = fast->next; //��ָ������һ��
		if (fast == slow)//
		{
			meet = fast;
			break;
		}	
	}
	if (meet == NULL)
	{
		cout << "û������" << endl;
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


