#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <map>
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */

//½á¹¹Ìå
struct Node
{
	int data;
	Node* next, * random;
	Node(int x) :data(x), next(NULL), random(NULL) {}
};

int main()
{
	map<Node*, int> test;
	Node a(5);
	Node b(6);
	Node c(7);
	a.next = &b;
	b.next = &c;
	a.random = &c;
	b.random = &a;
	c.random = &c;
	test[&a] = 1;
	test[&b] = 2;
	test[&c] = 3;
	cout << "a:" << test[a.next] << endl << "b:" << test[b.random] << endl << "c:" << test[c.random] << endl;
	system("pause");
	return  EXIT_SUCCESS;
}
#endif;
