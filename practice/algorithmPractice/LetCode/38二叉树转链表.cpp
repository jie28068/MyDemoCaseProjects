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
typedef struct Tree_Re
{
	int value;

	Tree_Re* left, * right;

	/*¹¹Ôìº¯Êý*/
	Tree_Re(int x) :value(x), left(NULL), right(NULL) {}
}tree;

class solution
{
public:
	void funv(tree* node)
	{
		vector<tree*> a;
		cun(a, node);
		for (int i = 1; i < a.size(); i++)
		{
			a[i - 1]->left = NULL;
			a[i - 1]->right = a[i];
		}
	}

private:
	void cun(vector<tree*>& a, tree* node)
	{
		if (!node)
		{
			return;
		}
		a.push_back(node);
		cun(a, node->left);
		cun(a, node->right);
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
	s.funv(&a);
	tree* head = &a;
	while (head)
	{
		cout << head->value << endl;
		head = head->right;
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
