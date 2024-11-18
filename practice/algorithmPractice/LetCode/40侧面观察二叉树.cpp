
#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <queue>
#include <vector>
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
	vector<int> view(tree* node)
	{
		//������������һ���ڵ�
		vector<int> vie;
		//<�ڵ㣬����>
		queue<pair<tree*, int>> qu;
		//for (int i = 0; i < 4; i++)
		//{
		//	vie.push_back(0);
		//}
		
		if (node)
		{
			qu.push(make_pair(node, 0));
		}
		while (!qu.empty())
		{
			//��ȡ�ڵ������
			tree* root = qu.front().first;
			int depth = qu.front().second;

			//����
			qu.pop();

			//Ϊ�˿��������ռ�
			if (vie.size() == depth)
			{
				vie.push_back(root->value);

			}
			//���¸ò�����һ����
			else
			{
				vie[depth] = root->value;
			}

			if (root->left)
			{
				qu.push(make_pair(root->left, depth + 1));
			}
			if (root->right)
			{
				qu.push(make_pair(root->right, depth + 1));
			}
		}
		return vie;
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
	vector<int> num = s.view(&a);
	for (int i = 0; i < num.size(); i++)
	{
		cout << num[i] << endl;
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
