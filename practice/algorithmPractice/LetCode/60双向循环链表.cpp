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

//节点结构体
struct ListNode
{
	//指针域
	struct ListNode* per;

	struct ListNode* pnext;
	//数据域
	int data;

}p_node;

//头结点
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
		cout << "构造成功" << endl;
	}

	//生成节点
	void CreatList()
	{
		int a = 0;
		cout << "请输入值！0为结束" << endl;
		while (1)
		{
			cin >> a;
			if (a == 0)
			{
				cout << "添加完成" << endl;
				break;
			}

			//创建新的节点存放数据
			struct ListNode* pnew = new struct ListNode;
			pnew->data = a;
			pnew->per = nullptr;
			pnew->pnext = nullptr;

			//头结点为空
			if (head->pend == NULL)
			{
				head->pend = pnew;
				head->pfirst = pnew;
			}
			else
			{
				//尾插
				head->pend->pnext = pnew;
				pnew->per = head->pend;
				head->pend = pnew;
			}
			head->number++;
		}
	}

	//打印
	void PrintList()
	{
		cout << "节点现有数:" << head->number << endl;
		for (struct ListNode* p = head->pfirst; p != NULL; p = p->pnext)
		{
			cout << p->data <<"  " ;
		}
		cout << endl;
	}

	//删除节点
	void DeleteList()
	{
		int a = 0;
		cin >> a;
		if (head->pend == NULL)
		{
			cout << "空链表！ " << endl;
			return;
		}
		struct ListNode* p = head->pfirst;
		while (p)
		{
			//判断，为头
			if (p->data == head->pfirst->data)
			{
				head->pfirst = p->pnext;
				p->pnext->per = nullptr;
				p->pnext = nullptr;
				head->number--;
				p = p->pnext;
			}
			//为尾
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
				cout << "没找到！" << endl;
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

	//查找
	void SeachList()
	{
		int a = 0;
		cin >> a;
		struct ListNode* p = head->pfirst;
		while (p)
		{
			if (p->data == a)
			{
				cout << "找到了！" << endl;
				return;
			}
			p = p->pnext;
		}
		cout << "没找到！" << endl;
	}

	//插指定值后面
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
		//找不到尾插
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
		cout << "1.添加节点！" << endl;
		cout << "2.删除节点！" << endl;
		cout << "3.插入节点！" << endl;
		cout << "4.查找节点！" << endl;
		cout << "5.打印节点！" << endl;
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
