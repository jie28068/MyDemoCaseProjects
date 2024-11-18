#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <string>
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
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

	/*����תΪ�ַ���*/
	void change_int(int value, string& str)
	{
		string temp;
		while (value)
		{
			temp += value % 10 + '0';
			value = value / 10;
		}
		/*��ת����*/
		for (int i =(int)(temp.length() - 1); i >= 0; i--)
		{
			str += temp[i];
		}
		/*ÿ��ת��������#*/
		str += '#';
	}

	/*ǰ��������ڵ�ֵ��ŵ��ַ�����*/
	void preorder(tree* node, string& data)
	{
		if (!node)
		{
			return;
		}
		string str;
		change_int(node->value, str);
		data += str;
		preorder(node->left, data);
		preorder(node->right, data);
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

	/*���ַ���ת��Ϊ����,�����ɶ�����*/
	tree* change_string(string data)
	{
		if (data.length() == 0)
		{
			return NULL;
		}
		//��̬����Ȼ���ػᾯ��
		static vector<tree*> vec;
		int val = 0;
		for (int i = 0; i < data.length(); i++)
		{
			if (data[i] == '#')
			{
				/*��Ϊ�㣬�˵�ʱ����൱��������#*/
				vec.push_back(new tree(val));
				val = 0;
			}
			else
			{
				val = val * 10 + data[i] - '0';
			}
		}
		for (int i = 1; i < vec.size(); i++)
		{
			twotree(vec[0], vec[i]);
		}
		return vec[0];
	}
};
int main()
{
	solution s;
	tree a(8);
	tree b(3);
	tree c(10);
	tree d(1);
	tree e(6);
	tree f(15);
	a.left = &b;
	a.right = &c;
	b.left = &d;
	b.right = &e;
	c.right = &f;
	string str;
	s.preorder(&a, str);
	cout << str << endl;
	s.per_search(s.change_string(str), 0);

	system("pause");
	return  EXIT_SUCCESS;
}
#endif
