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
   * Explain:���Ѵ�ż�����
 */

bool cmp(pair<int, int>& a, pair<int, int>& b)
{
	return a.first > b.first;
}
class func
{
public:
	/*lΪ��㵽�յ�ľ��룬pΪ��ʼ����������pair<����վ���յ�ľ��룬����վ��������>*/
	int change(int L, int p, vector<pair<int, int>>& stop)
	{
		/*������*/
		priority_queue<int >Q;  //Ĭ������

		/*��¼���˼�����*/
		int result = 0;

		/*���յ���Ϊһ��ͣ���㣬�Ž�����*/
		stop.push_back(make_pair(0, 0));

		/*��ͣ���㵽�յ�ľ��밴��С����*/
		sort(stop.begin(), stop.end(), cmp);

		/*����*/
		for (int i = 0; i < stop.size(); i++)
		{
			/*��ǰҪ�ߵľ���*/
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

			/*���µ�ǰ�㵽�յ�ľ���*/
			L = stop[i].first;

			/*����ǰ��������ӵ�����*/
			Q.push(stop[i].second);
		}
	}



};
int main()
{
	vector<pair<int, int>> stop;
	int N, L, P, distance, fuel;
	cout << "���ξ���" << endl;
	cin >> N;
	for (int i = 0; i < N; i++)
	{
		cout << "����վ�ľ���ͼ�����" << endl;
		cin >> distance >> fuel;
		stop.push_back(make_pair(distance, fuel));
	}
	cout << "���յ�������ʼ������" << endl;
	cin >> L >> P;
	func f;
	cout << f.change(L, P, stop);

	system("pause");
	return  EXIT_SUCCESS;
}
#endif
