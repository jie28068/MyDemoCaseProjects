#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
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
	 Tree_Re(int x):value(x),left(NULL),right(NULL){}
}tree;

class soluton
{
public:
	void func(tree* t, int num)
	{
		if (!t)
		{
			return;
		}
		
		for (int i = 0; i < num; i++)
		{
			cout << "---";
		}
		
		/*打印的位置不同，就绝对前中后序*/
		printf("%d\n", t->value);//
		func(t->left, num + 1);
		func(t->right, num + 1);

	}

};
int main()
{
	tree a(1);
	tree b(2);
	tree c(3);
	tree d(4);
	tree e(5);
	tree f(6);

	a.left = &b;
	a.right = &c;
	b.left = &d;
	b.right = &e;
	c.right = &f;

	soluton s;
	s.func(&a, 0);
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
