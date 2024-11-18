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
 /*结构体*/
typedef struct Tree_Re
{
	int value;

	Tree_Re* left, * right;

	/*构造函数*/
	Tree_Re(int x) :value(x), left(NULL), right(NULL) {}
}tree;

class solution
{
public:
	/*生成二叉排序树*/
	void twotree(tree* node, tree* intnode)
	{
		/*插左边*/
		if (node->value > intnode->value)
		{
			if (node->left)
			{
				/*不为空继续递归*/
				twotree(node->left, intnode);
			}
			else
			{
				/*为空插上*/
				node->left = intnode;
			}
		}
		/*插右边*/
		else
		{
			if (node->right)
			{
				/*不为空继续递归*/
				twotree(node->right, intnode);
			}
			else
			{
				/*为空插上*/
				node->right = intnode;
			}
		}
	}

	/*数字转为字符串*/
	void change_int(int value, string& str)
	{
		string temp;
		while (value)
		{
			temp += value % 10 + '0';
			value = value / 10;
		}
		/*翻转回来*/
		for (int i =(int)(temp.length() - 1); i >= 0; i--)
		{
			str += temp[i];
		}
		/*每次转换都加上#*/
		str += '#';
	}

	/*前序遍历将节点值存放到字符串中*/
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

	/*先序遍历*/
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

	/*将字符串转换为数字,并生成二叉树*/
	tree* change_string(string data)
	{
		if (data.length() == 0)
		{
			return NULL;
		}
		//静态，不然返回会警告
		static vector<tree*> vec;
		int val = 0;
		for (int i = 0; i < data.length(); i++)
		{
			if (data[i] == '#')
			{
				/*赋为零，乘的时候就相当于跳过了#*/
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
