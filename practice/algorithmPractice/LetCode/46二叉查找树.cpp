#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:给与一个乱序数组，生成一个二叉排序树
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

};
int main()
{
	solution s;
	tree node(8);
	vector<tree*> vec;
	int t[] = { 3,10,1,6,15,5,7 };
	for (int i = 0; i < 7; i++)
	{
		/*匿名对象*/
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
