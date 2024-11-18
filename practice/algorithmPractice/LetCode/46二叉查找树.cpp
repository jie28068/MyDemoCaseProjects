#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:����һ���������飬����һ������������
   * Explain:
 */

 /*�ṹ��*/
typedef struct Tree_Re
{
	int value;

	Tree_Re* left, * right;

	/*���캯��*/
	Tree_Re(int x) :value(x), left(NULL), right(NULL) {}
}tree;

class solution
{
public:
	/*���ɶ���������*/
	void twotree(tree* node, tree* intnode)
	{
		/*�����*/
		if (node->value > intnode->value)
		{
			if (node->left)
			{
				/*��Ϊ�ռ����ݹ�*/
				twotree(node->left, intnode);
			}
			else
			{
				/*Ϊ�ղ���*/
				node->left = intnode;
			}
		}
		/*���ұ�*/
		else
		{
			if (node->right)
			{
				/*��Ϊ�ռ����ݹ�*/
				twotree(node->right, intnode);
			}
			else
			{
				/*Ϊ�ղ���*/
				node->right = intnode;
			}
		}
	}
	/*�������*/
	void per_search(tree* node, int x)
	{
		if (!node)
		{
			return;
		}
		for (int i = 0; i < x; i++)
		{
			cout << "---";
		}
		cout << node->value << endl;
		per_search(node->left, x + 1);
		per_search(node->right, x + 1);
	}

};
int main()
{
	solution s;
	tree node(8);
	vector<tree*> vec;
	int t[] = { 3,10,1,6,15,5,7 };
	for (int i = 0; i < 7; i++)
	{
		/*��������*/
		vec.push_back(new tree(t[i]));
	}
	for (int i = 0; i < vec.size(); i++)
	{
		s.twotree(&node, vec[i]);
	}

	s.per_search(&node, 0);
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
