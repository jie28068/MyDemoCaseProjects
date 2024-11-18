#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:给定一个数组，在某一位置其后面的数字比其小的数字有多少个，并将其值写到一个新的数组中并输出
   * Explain:eg:[5,-7,9,1,3,5-2,1] --->  [5,0,5,1,2,2,0,0]
   * 主要是记录左子树的数量，即比该点小的数量
 */

/*树结构体*/
typedef struct Treenode
{
	int value;
	/*记录左子树的数量*/
	int count;

	Treenode* left, * right;

	Treenode(int x) :value(x), left(NULL), right(NULL), count(0) {}
}tree;

class solution
{
public:
	vector<int> countsmaller(vector<int>& nums)
	{
		/*最终结果*/
		vector<int> result;

		/*二叉查找树节点池*/
		vector<tree*> vec;

		/*比当前节点的小的数组*/
		vector<int> count;

		for (int i = nums.size() - 1; i >= 0; i--)
		{
			vec.push_back(new tree(nums[i]));
		}

		/*第一节点的count为0*/
		count.push_back(0);

		/*调用生成二叉树*/
		for (int i = 1; i < vec.size(); i++)
		{
			int count_small = 0;
			BST_insert(vec[0], vec[i], count_small);
			/*记录每个节点的左子树数量*/
			count.push_back(count_small);
		}

		for (int i = vec.size() - 1; i >= 0; i--)
		{
			delete vec[i];
			result.push_back(count[i]);
		}
		return result;
	}

	/*生成二叉树*/
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
		/*内置类型不需要new开辟空间*/
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
