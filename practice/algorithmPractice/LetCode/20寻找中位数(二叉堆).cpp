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
#include <queue>
#include <vector>

class Func
{
public:
	/*最小堆*/
	priority_queue<int, vector<int>, greater<int>> samll_heap;

	/*最大堆*/
	priority_queue<int, vector<int>, less<int>> big_heap;

	void addNum(int x)
	{
		/*为空时*/
		if (big_heap.empty())
		{
			big_heap.push(x);
			return;
		}

		/*数量相同时*/
		if (samll_heap.size() == big_heap.size())
		{	
			if (x < big_heap.top())
			{
				big_heap.push(x);
			}
			else
			{
				samll_heap.push(x);
			}
		}

		/*最大堆多与最小堆时*/
		else if (big_heap.size() > samll_heap.size())
		{
			if (x > big_heap.top())
			{
				samll_heap.push(x);
			}
			else
			{
				/*将最大堆的堆顶元素放入最小堆*/
				samll_heap.push(big_heap.top());
				big_heap.pop();
				big_heap.push(x);
			}
		}

		/*最小堆多于最大堆时*/
		else if (big_heap.size() < samll_heap.size())
		{
			if (x < samll_heap.top())
			{
				big_heap.push(x);
			}
			else
			{
				/*将最小堆的堆顶元素放入最大堆*/
				big_heap.push(samll_heap.top());
				samll_heap.pop();
				samll_heap.push(x);
			}
		}
	}
	
	//计算平均数
	double findmed()
	{
		if (big_heap.size() == samll_heap.size())
		{
			return (big_heap.top() + samll_heap.top()) / 2;
		}
		else if(big_heap.size() > samll_heap.size())
		{
			return big_heap.top();
		}
		return samll_heap.top();
	}

};
int main()
{
	Func f;
	int test[] = { 6,10,1,7,99,4,33 };
	for (int i = 0; i < 7; i++)
	{
		f.addNum(test[i]);
		cout << f.findmed() << endl;
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
