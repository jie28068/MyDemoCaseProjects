#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <map>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */

//�ڵ�ṹ��
struct ListNode
{
	//ָ����
	struct ListNode* per;

	struct ListNode* pnext;
	//������
	int data;

}p_node;

//ͷ���
struct HeadListNode
{
	struct ListNode* pfirst;
	struct ListNode* pend;

	int number;
}p_head;


class Solution
{
public:
	Solution():head(new struct HeadListNode)
	{
		head->number = 0;
		head->pend = nullptr;
		head->pfirst = nullptr;
		cout << "����ɹ�" << endl;
	}

	//���ɽڵ�
	void CreatList()
	{
		int a = 0;
		cout << "������ֵ��0Ϊ����" << endl;
		while (1)
		{
			cin >> a;
			if (a == 0)
			{
				cout << "������" << endl;
				break;
			}

			//�����µĽڵ�������
			struct ListNode* pnew = new struct ListNode;
			pnew->data = a;
			pnew->per = nullptr;
			pnew->pnext = nullptr;

			//ͷ���Ϊ��
			if (head->pend == NULL)
			{
				head->pend = pnew;
				head->pfirst = pnew;
			}
			else
			{
				//β��
				head->pend->pnext = pnew;
				pnew->per = head->pend;
				head->pend = pnew;
			}
			head->number++;
		}
	}

	//��ӡ
	void PrintList()
	{
		cout << "�ڵ�������:" << head->number << endl;
		for (struct ListNode* p = head->pfirst; p != NULL; p = p->pnext)
		{
			cout << p->data <<"  " ;
		}
		cout << endl;
	}

	//ɾ���ڵ�
	void DeleteList()
	{
		int a = 0;
		cin >> a;
		if (head->pend == NULL)
		{
			cout << "������ " << endl;
			return;
		}
		struct ListNode* p = head->pfirst;
		while (p)
		{
			//�жϣ�Ϊͷ
			if (p->data == head->pfirst->data)
			{
				head->pfirst = p->pnext;
				p->pnext->per = nullptr;
				p->pnext = nullptr;
				head->number--;
				p = p->pnext;
			}
			//Ϊβ
			else if (p == head->pend)
			{
				head->pend = p->per;
				p->per = nullptr;
				head->pend->pnext = nullptr;
				head->number--;
				p = p->pnext;
			}
			else if (p == NULL)
			{
				cout << "û�ҵ���" << endl;
			}
			else
			{
				p->per->pnext = p->pnext;
				p->pnext->per = p->per;
				p->per = nullptr;
				p->pnext = nullptr;
				p = p->pnext;
				head->number--;
			}
		}

	}

	//����
	void SeachList()
	{
		int a = 0;
		cin >> a;
		struct ListNode* p = head->pfirst;
		while (p)
		{
			if (p->data == a)
			{
				cout << "�ҵ��ˣ�" << endl;
				return;
			}
			p = p->pnext;
		}
		cout << "û�ҵ���" << endl;
	}

	//��ָ��ֵ����
	void SetList()
	{
		int a = 0;
		cin >> a;
		struct ListNode* p = head->pfirst;
		while (p)
		{
			if (p->data == a)
			{
				int b = 0;
				cin >> b;
				struct ListNode* pnew = new struct ListNode;
				pnew->data = b;
				pnew->per = nullptr;
				pnew->pnext = nullptr;
				p->pnext->per = pnew;
				pnew->pnext = p->pnext;
				p->pnext = pnew;
				pnew->per = p;			
			}
			p = p->pnext;
		}
		//�Ҳ���β��
	}
private:
	struct HeadListNode* head;
};

int main()
{
	Solution s;
	int a = 0;
	while (1)
	{
		cout << "1.��ӽڵ㣡" << endl;
		cout << "2.ɾ���ڵ㣡" << endl;
		cout << "3.����ڵ㣡" << endl;
		cout << "4.���ҽڵ㣡" << endl;
		cout << "5.��ӡ�ڵ㣡" << endl;
		cin >> a;
		switch (a)
		{
		case 1:
			s.CreatList();
			break;
		case 2:
			s.DeleteList();
			break;
		case 3:
			s.SetList();
			break;
		case 4:
			s.SeachList();
			break;
		case 5:
			s.PrintList();
			break;
		default:
			break;
		}

	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
