#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */

typedef char ch;

//ջ������
int top = -1;

//����ṹ��
typedef struct BinTree
{
	//������
	ch data;
	//ָ�������Һ���
	struct BinTree* lchild, * rchild;
}Tree;

//����һ����
class ClassTree
{
public:
	//����һ��������
	Tree* CreatTree();

	//�������
	void First(Tree* tree);

	//��ջ����
	void push(Tree** a, Tree* b);

	//��ջ����
	void pop();

	//��ȡջ��Ԫ��
	Tree* gettop(Tree** a);

	//�������������ջ��ʽ
	void proT(Tree* tree);

	//���
	void put(Tree* tree);

};

//����һ��������
Tree* ClassTree::CreatTree()
{
	Tree* tree = new Tree;

	tree->data = 'A';
	tree->lchild = new Tree;
	tree->rchild = new Tree;

	tree->lchild->data = 'B';
	tree->lchild->lchild = new Tree;
	tree->lchild->rchild = new Tree;

	tree->rchild->data = 'C';
	tree->rchild->lchild = NULL;
	tree->rchild->rchild = NULL;

	tree->lchild->lchild->data = 'D';
	tree->lchild->lchild->rchild = NULL;
	tree->lchild->lchild->lchild = NULL;

	tree->lchild->rchild->data = 'E';
	tree->lchild->rchild->lchild = NULL;
	tree->lchild->rchild->rchild = new Tree;

	tree->lchild->rchild->rchild->data = 'F';
	tree->lchild->rchild->rchild->lchild = NULL;
	tree->lchild->rchild->rchild->rchild = NULL;

	return tree;
}

//����
void ClassTree::First(Tree* tree)
{
	if (tree == NULL)
	{
		return;
	}
	
	cout << tree->data << endl;
	First(tree->lchild);
	First(tree->rchild);
}

//��ջ����
void ClassTree::push(Tree** a, Tree* b)
{
	a[++top] = b;
}

//��ջ����
void ClassTree::pop()
{
	if (-1 == top)
	{
		return;
	}
	--top;
}

//��ȡջ��
Tree* ClassTree::gettop(Tree** a)
{
	return a[top];
}

//�������������ջ��ʽ
void ClassTree::proT(Tree* tree)
{
	Tree* a[10];
	Tree* p;              //����ָ��
	push(a, tree);    //��ջ
	while (top != -1)
	{
		p = gettop(a);   //��ȡջ��
		pop();           //��ջ
		while (p)
		{
			put(p);      //��ӡ
			if (p->rchild)
			{
				push(a, p->rchild);
			}
			p = p->lchild;
		}
	}
}

//��ӡ
void ClassTree::put(Tree* tree)
{
	cout << tree->data << endl;
}

int main()
{
	ClassTree t;
	Tree* tree = t.CreatTree();
	t.First(tree);
	cout << "*******************" << endl;
	t.proT(tree);

	system("pause");
	return  EXIT_SUCCESS;
}
#endif;


