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
	l_list* ni_list(l_list* head);
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
		int a=0;
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
l_list* List::ni_list(l_list* head)
{
	//˼�룺�����ת����������ָ��һ��ָ��ͷ��һ����������
	l_list* new_head = NULL;	
	while (head)
	{
		l_list* n = head->next;  //����ָ��
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