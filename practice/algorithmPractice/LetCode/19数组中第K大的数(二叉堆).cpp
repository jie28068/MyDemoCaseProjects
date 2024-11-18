#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:以知未排序的数组，求第K大的数字
   * 维护一个K大小的最小堆，当堆内元素小于K时直接入堆，当堆顶元素小于新进元素，弹出堆顶
   * 堆顶就是第K大的元素
   * 
 */
#include <queue>
class Func
{
public:
	int find_K(vector<int>& num, int K)
	{
		/*构造最小堆*/
		priority_queue<int, vector<int>, greater<int>> Q;
		for (int i = 0; i < num.size(); i++)
		{
			/*如果堆内数量小于K,直接入堆*/
			if (Q.size() < K)
			{
				Q.push(num[i]);
			}
			/*维护堆顶元素*/
			else if (Q.top() < num[i])
			{
				Q.pop();
				Q.push(num[i]);
			}
		}
		return Q.top();
	}
};

int main()
{
	Func f;
	vector<int> num;
	num.push_back(3);
	num.push_back(2);
	num.push_back(1);
	num.push_back(5);
	num.push_back(6);
	num.push_back(4);
	cout<<f.find_K(num, 3)<<endl;
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
