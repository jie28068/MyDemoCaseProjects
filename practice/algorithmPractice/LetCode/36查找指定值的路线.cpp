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
	vector<vector<int>> func(tree* node, int sum)
	{
		/*���*/
		vector<vector<int>> result;
		/*��ջ*/
		vector<int> path;
		int pvalue = 0;
		findpath(node, sum, pvalue, path, result);
		return result;
	}

private:
	void findpath(tree* node, int sum, int pvalue, vector<int>& path, vector<vector<int>>& result)
	{
		/*��������*/
		if (!node)
		{
			return;
		}
		/*ѹջ*/
		path.push_back(node->value);
		/*��ֵ*/
		pvalue += node->value;
		/*��ֵ*/
		if (!node->left && !node->right && pvalue == sum)
		{
			result.push_back(path);
		}
		/*�ݹ�*/
		findpath(node->left, sum, pvalue, path, result);
		findpath(node->right, sum, pvalue, path, result);
		/*��ֵ*/
		pvalue -= node->value;
		/*��ջ*/
		path.pop_back();
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
	vector<vector<int>> result = s.func(&a, 22);
	for (int i = 0; i < result.size(); i++)
	{
		printf("i=%d\n", i);
		for (int j = 0; j < result[i].size(); j++)
		{
			printf("%d ", result[i][j]);
		}
		printf("\n");
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
