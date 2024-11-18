#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <queue>
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

/**/
class solution
{
public:
	void BFS_func(tree* root)
	{
		queue<tree*> Q;
		Q.push(root);
		while (!Q.empty())
		{
			tree* node = Q.front();
			Q.pop();
			cout << node->value << endl;
			if (node->left)
			{
				Q.push(node->left);
			}
			if (node->right)
			{
				Q.push(node->right);
			}
		}
	}
};
int main()
{
	tree a(5);
	tree b(4);
	tree c(8);
	tree d(11);
	tree e(13);
	tree f(4);
	tree g(7);
	tree h(2);
	tree i(5);
	tree k(1);
	a.left = &b;
	a.right = &c;
	b.left = &d;
	d.left = &g;
	d.right = &h;
	c.left = &e;
	c.right = &f;
	f.left = &i;
	f.right = &k;
	solution s;
	s.BFS_func(&a);

	system("pause");
	return  EXIT_SUCCESS;
}
#endif
