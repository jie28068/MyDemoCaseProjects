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

//栈顶数据
int top = -1;

//定义结构体
typedef struct BinTree
{
	//数据域
	ch data;
	//指针域，左右孩子
	struct BinTree* lchild, * rchild;
}Tree;

//定义一个类
class ClassTree
{
public:
	//创建一个二叉树
	Tree* CreatTree();

	//先序遍历
	void First(Tree* tree);

	//入栈函数
	void push(Tree** a, Tree* b);

	//出栈函数
	void pop();

	//获取栈顶元素
	Tree* gettop(Tree** a);

	//先序遍历，出入栈方式
	void proT(Tree* tree);

	//输出
	void put(Tree* tree);

};

//创建一个二叉树
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

//先序
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

//入栈函数
void ClassTree::push(Tree** a, Tree* b)
{
	a[++top] = b;
}

//出栈函数
void ClassTree::pop()
{
	if (-1 == top)
	{
		return;
	}
	--top;
}

//获取栈顶
Tree* ClassTree::gettop(Tree** a)
{
	return a[top];
}

//先序遍历，出入栈方式
void ClassTree::proT(Tree* tree)
{
	Tree* a[10];
	Tree* p;              //遍历指针
	push(a, tree);    //入栈
	while (top != -1)
	{
		p = gettop(a);   //获取栈顶
		pop();           //出栈
		while (p)
		{
			put(p);      //打印
			if (p->rchild)
			{
				push(a, p->rchild);
			}
			p = p->lchild;
		}
	}
}

//打印
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


