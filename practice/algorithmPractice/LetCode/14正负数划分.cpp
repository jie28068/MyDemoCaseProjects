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
	//������
	int data;
	//ָ����
	s_list* next;
	////���캯��
	//s_list(int x) :data(x), next(NULL) {}
}l_list;

class list
{
public:
	//��������
	l_list* make()
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

			//���������β��
			if (a > 0)
			{
				//β��
				node->next = NULL;
				node->data = a;
				p->next = node;
				p = node;
			}
			else if(a < 0)
			{
				//ͷ��
				node->data = a;				
				node->next = head->next;
				head->next = node;			
			}

		}
		return head->next;
	}

	//��ӡ����
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
