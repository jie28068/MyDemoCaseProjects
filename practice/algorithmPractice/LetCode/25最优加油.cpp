#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:最大堆存放加油量
 */

bool cmp(pair<int, int>& a, pair<int, int>& b)
{
	return a.first > b.first;
}
class func
{
public:
	/*l为起点到终点的距离，p为初始的汽油量，pair<加油站到终点的距离，加油站的汽油量>*/
	int change(int L, int p, vector<pair<int, int>>& stop)
	{
		/*存油量*/
		priority_queue<int >Q;  //默认最大堆

		/*记录加了几次油*/
		int result = 0;

		/*将终点作为一个停靠点，放进容器*/
		stop.push_back(make_pair(0, 0));

		/*将停靠点到终点的距离按大到小排序*/
		sort(stop.begin(), stop.end(), cmp);

		/*遍历*/
		for (int i = 0; i < stop.size(); i++)
		{
			/*当前要走的距离*/
			int dis = L - stop[i].first;

			while (!Q.empty() && p < dis)
			{
				p += Q.top();
				Q.pop();
				result++;
			}
			if (Q.empty() && p < dis)
			{
				return -1;
			}
			p = p - dis;

			/*更新当前点到终点的距离*/
			L = stop[i].first;

			/*将当前的汽油添加到最大堆*/
			Q.push(stop[i].second);
		}
	}



};
int main()
{
	vector<pair<int, int>> stop;
	int N, L, P, distance, fuel;
	cout << "几段距离" << endl;
	cin >> N;
	for (int i = 0; i < N; i++)
	{
		cout << "加油站的距离和加油量" << endl;
		cin >> distance >> fuel;
		stop.push_back(make_pair(distance, fuel));
	}
	cout << "到终点距离和起始汽油量" << endl;
	cin >> L >> P;
	func f;
	cout << f.change(L, P, stop);

	system("pause");
	return  EXIT_SUCCESS;
}
#endif
