#if 0
#include <iostream>
using namespace std;

/*����һ�����֣������д��ڴ����ķ�ǰ�棬С�ڵķź��棬���ǲ��ı�ԭ�е�������ʽ*/
//�ṹ��
typedef struct s_list
{
	//������
	int data;
	//ָ����
	s_list* next;
	//���캯��
	s_list(int x) :data(x), next(NULL) {}
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

	//������
	l_list* h_f(l_list* p);
};

//��������
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
		//β��
		node->next = NULL;

		p->next = node;
		p = node;

	}
	return head->next;
}

//��ӡ����
void List::pr_list(l_list* p)
{
	while (p)
	{
		cout << "****************" << endl;
		cout <<p->data << endl;
		p = p->next;
	}
}

//��������
l_list* List::h_f(l_list* p)
{
	//������������
	l_list* p1 = new l_list(0);
	p1->next = NULL;
	l_list* pp1 = p1;
	l_list* p2 = new l_list(0);
	p2->next = NULL;
	l_list* pp2 = p2;
	int x = 3;
	//ѭ��
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