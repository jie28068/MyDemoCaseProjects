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
 //�ṹ��
typedef struct Lis
{
	//ָ����
	struct Lis* next;
	//������
	int date;
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

	//��������
	l_list* ni_list(l_list* head ,int m ,int n);
};

//��������
l_list* List::make_list()
{
	cout << "������һ�����֣���0����" << endl;
	//����һ���ƽڵ�
	l_list* head = new l_list;
	head->next = NULL;
	//����һ������ָ��
	l_list* p = NULL;
	p = head;
	while (1)
	{
		int a = 0;
		cin >> a;
		if (a == 0)
			break;
		l_list* node = new l_list;
		//β��
		node->next = NULL;
		node->date = a;

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
		cout << p->date << endl;
		p = p->next;
	}
}

//��������
l_list* List::ni_list(l_list* p ,int m ,int n)
{
	if (p == NULL)
	{
		return;
	}
	int len = n - m + 1; //����Ҫ���õĳ���
	l_list* head = p;    //��¼ͷ���
	l_list* pre = NULL;  //Ҫ���õ�ǰ���ڵ�
	while (p && (--m))
	{
		 pre = p;
		p = p->next;
	}
	l_list* temp = p; //��¼����ͷ�ڵ�
	l_list* newp = NULL; //������ʱ���ָ��
	while (--len)
	{
		l_list* next = p->next; //�������ָ��
		p->next = newp;     //ÿ�ó�һ����ָ��ǰһ��
		newp = p;
		p = next;
	}
	temp->next = p;
	if (pre)
	{
		pre->next = newp;
	}
	else          //������õ��ǵ�һ���ڵ�
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
