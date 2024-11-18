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
//�������ڵ�
typedef struct treenode
{
	//������
	datadef data;
	//ָ����
	struct treenode* lchild;
	struct treenode* rchild;
}tn;
//#define NODE struct treenode
//#include "drawtree.h"
//��������
tn* init_tree(tn* root, datadef data)
{
	//����һ������ָ��
	tn* p = root;
	//��ʼ���ڵ�
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
			if (data < p->data)//С�ڲ����
			{
				if (p->lchild == NULL)//�ҵ���
				{
					p->lchild = pnew;
					break;
				}
				else//û�ҵ��ռ�����
				{
					p = p->lchild;
				}
			}
			else if (data > p->data)//���ڲ��ұ�
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
//�������
void first_tree(tn* root)
{
	if (root == NULL)
	{
		return;
	}
	//��
	printf("%c", root->data);
	//��
	first_tree(root->lchild);
	//��
	first_tree(root->rchild);
}
//�������
void mid_tree(tn* root)
{
	if (root == NULL)
	{
		return ;
	}
	//��
	mid_tree(root->lchild);
	//��
	printf("%c", root->data);
	//��
	mid_tree(root->rchild);
}
//�������
void last_tree(tn* root)
{
	if (root == NULL)
	{
		return ;
	}
	//��
	last_tree(root->lchild);
	//��
	last_tree(root->rchild);
	//��
	printf("%c", root->data);
	return;
}
//����ڵ�
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
//ɾ���ڵ�
/*tn* delete_tree(tn* root ,int data)
{
	//�������ָ��
	tn* p = root;
	//�������ָ���ǰ��ָ��
	tn* pre = NULL;
	//����ָ��
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
	//û���ҵ�
	if (p == NULL)
	{
		return root;
	}
	//�ҵ��ˣ���һ����� Pû���ӽڵ�
	if (p->lchild == NULL && p->rchild == NULL)
	{
		//p���Ǹ�
		if (p == root)
		{
			free(p);
			return NULL;
		}
		//p����ڵ�
		else if (p == pre->lchild)
		{
			pre->lchild = NULL;
			free(p);
		}
		//p���ҽڵ�
		else if (p == pre->rchild)
		{
			pre->rchild = NULL;
			free(p);
		}
	}
	//�ڶ��������p�������ڵ�
	else if (p->lchild && p->rchild)
	{
		//�ҵ�p��ڵ�����ֵ
		//������һ��p2,��pre2
		tn* p2 = p->lchild;
		//��������Ҷ�ĺ���ڵ�
		tn* pre2 = NULL;
		//����P��ڵ���ҽڵ�
		while (p2->rchild)
		{
			pre2 = p2;
			p2 = p2->rchild;
		}
		//P����ڵ�ĵ�һ�������p2û���ҽڵ�
		if (p2 ==p->lchild)
		{	
			//��������
			int tmp;
			tmp = p2->data;
			p2->data = p->data;
			p->data = tmp;
			//�任ָ��ָ��
			p->lchild = p2->lchild;
			p2->lchild = NULL;
			free(p2);
		}
		//�ڶ��������p2���ҽڵ�
		else 
		{
			//������ֵ
			int tmp;
			tmp = p2->data;
			p2->data = p->data;
			p->data = tmp;
			//�任ָ��ָ��
			pre2->rchild = p2->lchild;
			p2->lchild = NULL;
			free(p2);
		}
	}
	//�����������p��һ���ڵ�
	else
	{
		if (p->lchild) // ֻ������
		{
			if (p == root) // ���ڵ�
			{
				root = p->lchild;
				p->lchild = NULL;
				free(p);
			}
			else if (p == pre->lchild) // p��pre������
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
		else if (p->rchild) //pֻ���Һ���
		{
			if (p == root) // ���ڵ�
			{
				root = p->rchild;
				p->rchild = NULL;
				free(p);
			}
			else if (p == pre->lchild) // p��pre������
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
//�ݹ�ɾ��
tn* Remove_Node(tn* root, datadef data)
{
	if (root == NULL)
		return  NULL;

	// ������������Ҫɾ���Ľڵ�
	if (data < root->data)
	{
		root->lchild = Remove_Node(root->lchild, data);
	}
	else if (data > root->data) // ������������
	{
		root->rchild = Remove_Node(root->rchild, data);
	}
	else // ɾ�����ǵ�ǰ�ڵ�
	{
		// ��ǰ�ڵ�������
		if (root->lchild != NULL)
		{
			// �������ӵ����ֵ
			tn* p = NULL;
			for (p = root->lchild; p->rchild != NULL; p = p->rchild);

			// ��p�ڵ���root��ֵ����
			root->data = p->data;

			// ��pɾ��   �ݹ鵽���һ��Ҷ֦�ڵ㣬��ʵɾ����Ҳ����Щֵ
			root->lchild = Remove_Node(root->lchild, p->data);
		}
		else if (root->rchild != NULL)
		{
			// �����Һ��ӵ���Сֵ
			tn* p = NULL;
			for (p = root->rchild; p->lchild != NULL; p = p->lchild);

			// ��p�ڵ���root��ֵ����
			root->data = p->data;

			// ��pɾ��
			root->rchild = Remove_Node(root->rchild, p->data);
		}
		else // û���κ��ӽڵ�
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