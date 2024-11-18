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
	/*������ֵ*/
	bool searchtree(tree* node, int date)
	{
		/*��ǰֵ���*/
		if (node->value == date)
		{
			return true;
		}
		/*���*/
		else if (date < node->value)
		{
			/*��߲�Ϊ�վͼ����ݹ�*/
			if (node->left)
			{
				searchtree(node->left, date);
			}
			/*û���˾ͷ���û��*/
			else
			{
				return false;
			}
		}
		/*�ұ�ͬ��*/
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
			printf("%d�����еĵ�\n", i);
		}
		else
		{
			printf("%d�������еĵ�\n", i);
		}
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
