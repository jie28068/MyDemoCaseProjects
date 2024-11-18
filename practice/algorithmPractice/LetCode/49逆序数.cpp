#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:����һ�����飬��ĳһλ�����������ֱ���С�������ж��ٸ���������ֵд��һ���µ������в����
   * Explain:eg:[5,-7,9,1,3,5-2,1] --->  [5,0,5,1,2,2,0,0]
   * ��Ҫ�Ǽ�¼�����������������ȸõ�С������
 */

/*���ṹ��*/
typedef struct Treenode
{
	int value;
	/*��¼������������*/
	int count;

	Treenode* left, * right;

	Treenode(int x) :value(x), left(NULL), right(NULL), count(0) {}
}tree;

class solution
{
public:
	vector<int> countsmaller(vector<int>& nums)
	{
		/*���ս��*/
		vector<int> result;

		/*����������ڵ��*/
		vector<tree*> vec;

		/*�ȵ�ǰ�ڵ��С������*/
		vector<int> count;

		for (int i = nums.size() - 1; i >= 0; i--)
		{
			vec.push_back(new tree(nums[i]));
		}

		/*��һ�ڵ��countΪ0*/
		count.push_back(0);

		/*�������ɶ�����*/
		for (int i = 1; i < vec.size(); i++)
		{
			int count_small = 0;
			BST_insert(vec[0], vec[i], count_small);
			/*��¼ÿ���ڵ������������*/
			count.push_back(count_small);
		}

		for (int i = vec.size() - 1; i >= 0; i--)
		{
			delete vec[i];
			result.push_back(count[i]);
		}
		return result;
	}

	/*���ɶ�����*/
	void BST_insert(tree* node, tree* inode, int &count_small)
	{
		if (inode->value <= node->value)
		{
			node->count++;
			if (node->left)
			{
				BST_insert(node->left, inode, count_small);
			}
			else
			{
				node->left = inode;
			}
		}
		else
		{
			count_small += node->count + 1;
			if (node->right)
			{
				BST_insert(node->right, inode, count_small);
			}
			else
			{
				node->right = inode;
			}
		}
	}
};

int main()
{
	solution s;
	vector<int> nums;
	int t[] = {5,-7, 9, 1, 3, 5, -2, 1};
	for (int i = 0; i < 8; i++)
	{
		/*�������Ͳ���Ҫnew���ٿռ�*/
		nums.push_back(t[i]);
	}
	vector<int> result = s.countsmaller(nums);

	for (int i = 0; i <result.size(); i++)
	{
		printf("[%d]", result[i]);
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
