#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
#if 0
typedef char datadef;
//创建树节点
typedef struct treenode
{
	//数据域
	datadef data;
	//指针域
	struct treenode* lchild;
	struct treenode* rchild;
}tn;
//#define NODE struct treenode
//#include "drawtree.h"
//插入树中
tn* init_tree(tn* root, datadef data)
{
	//定义一个遍历指针
	tn* p = root;
	//初始化节点
	tn* pnew = malloc(sizeof(tn));
	if (NULL == pnew)
		return NULL;
	pnew->data = data;
	pnew->lchild = NULL;
	pnew->rchild = NULL;
	if (NULL == root)
	{
		root = pnew;
	}
	else
	{ 
		while (p)
		{
			if (data < p->data)//小于插左边
			{
				if (p->lchild == NULL)//找到了
				{
					p->lchild = pnew;
					break;
				}
				else//没找到空继续找
				{
					p = p->lchild;
				}
			}
			else if (data > p->data)//大于插右边
			{
				if (p->rchild == NULL)
				{
					p->rchild = pnew;
					break;
				}
				else
				{
					p = p->rchild;
				}
			}
			else
			{
				printf("error!\n");
				break;
			}
		}
	}
	return root;
}
//先序遍历
void first_tree(tn* root)
{
	if (root == NULL)
	{
		return;
	}
	//根
	printf("%c", root->data);
	//左
	first_tree(root->lchild);
	//右
	first_tree(root->rchild);
}
//中序遍历
void mid_tree(tn* root)
{
	if (root == NULL)
	{
		return ;
	}
	//左
	mid_tree(root->lchild);
	//根
	printf("%c", root->data);
	//右
	mid_tree(root->rchild);
}
//后序遍历
void last_tree(tn* root)
{
	if (root == NULL)
	{
		return ;
	}
	//左
	last_tree(root->lchild);
	//右
	last_tree(root->rchild);
	//根
	printf("%c", root->data);
	return;
}
//输入节点
tn* get_tree()
{
	tn* root = NULL;
	while (1)
	{
		datadef data = getchar();
		if (data == '\n')
			break;
		root = init_tree(root, data);
	}
	return root;
}
//删除节点
/*tn* delete_tree(tn* root ,int data)
{
	//定义遍历指针
	tn* p = root;
	//定义便利指针的前驱指针
	tn* pre = NULL;
	//遍历指针
	while (p)
	{
		if (data == p->data)
		{
			break;
		}
		else if (data > p->data)
		{
			pre = p;
			p = p->rchild;
		}
		else if (data < p->data)
		{
			pre = p;
			p = p->lchild;
		}
	}
	//没有找到
	if (p == NULL)
	{
		return root;
	}
	//找到了，第一种情况 P没有子节点
	if (p->lchild == NULL && p->rchild == NULL)
	{
		//p就是根
		if (p == root)
		{
			free(p);
			return NULL;
		}
		//p是左节点
		else if (p == pre->lchild)
		{
			pre->lchild = NULL;
			free(p);
		}
		//p是右节点
		else if (p == pre->rchild)
		{
			pre->rchild = NULL;
			free(p);
		}
	}
	//第二种情况，p有俩个节点
	else if (p->lchild && p->rchild)
	{
		//找到p左节点的最大值
		//在设置一个p2,和pre2
		tn* p2 = p->lchild;
		//保存右子叶的后面节点
		tn* pre2 = NULL;
		//遍历P左节点的右节点
		while (p2->rchild)
		{
			pre2 = p2;
			p2 = p2->rchild;
		}
		//P的左节点的第一种情况，p2没有右节点
		if (p2 ==p->lchild)
		{	
			//交换数据
			int tmp;
			tmp = p2->data;
			p2->data = p->data;
			p->data = tmp;
			//变换指针指向
			p->lchild = p2->lchild;
			p2->lchild = NULL;
			free(p2);
		}
		//第二种情况，p2有右节点
		else 
		{
			//交换数值
			int tmp;
			tmp = p2->data;
			p2->data = p->data;
			p->data = tmp;
			//变换指针指向
			pre2->rchild = p2->lchild;
			p2->lchild = NULL;
			free(p2);
		}
	}
	//第三种情况，p有一个节点
	else
	{
		if (p->lchild) // 只有左孩子
		{
			if (p == root) // 根节点
			{
				root = p->lchild;
				p->lchild = NULL;
				free(p);
			}
			else if (p == pre->lchild) // p是pre的左孩子
			{
				pre->lchild = p->lchild;
				p->lchild = NULL;
				free(p);
			}
			else if (p == pre->rchild)
			{
				pre->rchild = p->lchild;
				p->lchild = NULL;
				free(p);
			}
		}
		else if (p->rchild) //p只有右孩子
		{
			if (p == root) // 根节点
			{
				root = p->rchild;
				p->rchild = NULL;
				free(p);
			}
			else if (p == pre->lchild) // p是pre的左孩子
			{
				pre->lchild = p->rchild;
				p->rchild = NULL;
				free(p);
			}
			else if (p == pre->rchild)
			{
				pre->rchild = p->rchild;
				p->rchild = NULL;
				free(p);
			}
		}
	}
	return root;
}*/
//递归删除
tn* Remove_Node(tn* root, datadef data)
{
	if (root == NULL)
		return  NULL;

	// 从左子树找需要删除的节点
	if (data < root->data)
	{
		root->lchild = Remove_Node(root->lchild, data);
	}
	else if (data > root->data) // 从右子树上找
	{
		root->rchild = Remove_Node(root->rchild, data);
	}
	else // 删除的是当前节点
	{
		// 当前节点右左孩子
		if (root->lchild != NULL)
		{
			// 查找左孩子的最大值
			tn* p = NULL;
			for (p = root->lchild; p->rchild != NULL; p = p->rchild);

			// 将p节点与root的值交换
			root->data = p->data;

			// 将p删除   递归到最后一个叶枝节点，其实删除的也是这些值
			root->lchild = Remove_Node(root->lchild, p->data);
		}
		else if (root->rchild != NULL)
		{
			// 查找右孩子的最小值
			tn* p = NULL;
			for (p = root->rchild; p->lchild != NULL; p = p->lchild);

			// 将p节点与root的值交换
			root->data = p->data;

			// 将p删除
			root->rchild = Remove_Node(root->rchild, p->data);
		}
		else // 没有任何子节点
		{
			free(root);
			return NULL;
		}
	}

	return  root;
}
int main()
{
	tn* root = get_tree();
	root = Remove_Node(root, 'D');
	first_tree(root);
	printf("\n");
	system("pause");
	return 0;
}
#endif