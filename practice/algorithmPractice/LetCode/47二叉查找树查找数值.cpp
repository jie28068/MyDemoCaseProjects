#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
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
	/*查找数值*/
	bool searchtree(tree* node, int date)
	{
		/*当前值相等*/
		if (node->value == date)
		{
			return true;
		}
		/*左边*/
		else if (date < node->value)
		{
			/*左边不为空就继续递归*/
			if (node->left)
			{
				searchtree(node->left, date);
			}
			/*没有了就返回没有*/
			else
			{
				return false;
			}
		}
		/*右边同理*/
		else
		{
			if (node->right)
			{
				searchtree(node->right, date);
			}
			else
			{
				return false;
			}
		}

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
	for (int i = 0; i < 20; i++)
	{
		if (s.searchtree(&a, i))
		{
			printf("%d是树中的点\n", i);
		}
		else
		{
			printf("%d不是树中的点\n", i);
		}
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
