#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <queue>
#include <string>
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */

int main()
{
	/*默认构造的是最大堆*/
	priority_queue<int> big_heap;

	/*最小堆构造方法*/
	priority_queue<int, vector<int>, greater<int>> samll_heap;

	/*最大堆构造*/
	priority_queue<int, vector<int>, less <int>> big_heap2;

	int test[] = { 6,10,1,7,99,4,33 };
	for (int i = 0; i < 7; i++)
	{
		samll_heap.push(test[i]);
	}
	cout << samll_heap.top() << endl;
	samll_heap.pop();
	cout << samll_heap.top() << endl;


	for (int i = 0; i < 7; i++)
	{
		big_heap2.push(test[i]);
	}
	cout << "***********" << endl;
	cout << big_heap2.top() << endl;
	big_heap2.push(100);
	cout << big_heap2.top() << endl;


	system("pause");
	return  EXIT_SUCCESS;
}
#endif
