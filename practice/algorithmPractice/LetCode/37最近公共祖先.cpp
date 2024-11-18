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
	/*��ȡҪ���ҽڵ��·��*/
	void peroder(tree* node, tree* pnode, vector<tree*>& path, vector<tree*>& rsult, int falg)
	{
		if (!node || falg)
		{
			return;
		}
		path.push_back(node);
		if (node == pnode)
		{
			falg = 1;
			rsult = path;
		}
		peroder(node->left, pnode, path, rsult, falg);
		peroder(node->right, pnode, path, rsult, falg);
		path.pop_back();
	}
	/*�Ƚ�����·�����һ����ͬ�Ľڵ�*/
	tree* compare(tree* root, tree* p, tree* q)
	{
		/*��ȡq�ڵ�·��*/
		vector<tree*> p_path;
		/*��ȡp�ڵ�·��*/
		vector<tree*> q_path;
		/*��ʱջ*/
		vector<tree*> t_path;

		int finish = 0;
		peroder(root, q, t_path, q_path, finish);
		t_path.clear();
		peroder(root, p, t_path, p_path, finish);
		int len = 0;
		if (q_path.size() < p_path.size())
		{
			len = q_path.size();
		}
		else
		{
			len = p_path.size();
		}

		tree* result = 0;
		for (int i = 0; i < len; i++)
		{
			if (p_path[i] == q_path[i])
			{
				result = p_path[i];
			}
		}
		return result;
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
	tree* result = s.compare(&a, &e, &k);
	cout << result->value << endl;
	system("pause");
	return  EXIT_SUCCESS;
}
#endif