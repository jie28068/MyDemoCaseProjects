#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <set>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:���õݹ鵽����ų�����;�ͱ�target����Ӽ������ݼ�֦
 */
bool cmp(int& a, int& b)
{
	return a > b;
}
class solution
{
public:
	vector <vector<int>> subet(vector<int>& num, int target)
	{
		/*�Ӽ�����*/
		vector<int> item;
		/*ȥ������*/
		set<vector<int>> ret;
		/*�������*/
		vector<vector<int>> result;
		/*����*/
		sort(num.begin(), num.end(), cmp);
		/*�Ӽ���*/
		int sum = 0;
		/*�ݹ�*/
		gernate(0, num, item, result, ret, target, sum);

		/*��������*/
		vector<vector<int>> m_result;
		for (int i = 0; i < result.size(); i++)
		{
			int number = 0;
			for (int j = 0; j < result[i].size(); j++)
			{
				number += result[i][j];
			}
			if (number == target)
			{
				m_result.push_back(result[i]);
			}
		}
		return m_result;
	}
private:
	void gernate(int i, vector<int>& num, vector<int>& item, vector<vector<int>>& result, set<vector<int>>& ret, int target, int sum)
	{
		
		/*��������*/
		if (i >= num.size() || sum > target)
		{
			return;
		}
		/*�����Ӽ�*/
		sum += num[i];
		item.push_back(num[i]);
		/*����ȥ���������Ƿ��и���*/
		if (ret.find(item) == ret.end() && sum == target)
		{
			result.push_back(item);
			ret.insert(item);
		}
		gernate(i + 1, num, item, result, ret, target, sum);
		sum -= num[i];
		item.pop_back();
		gernate(i + 1, num, item, result, ret, target, sum);
	}
};

int main()
{
	solution s;
	vector<int> num;
	num.push_back(10);
	num.push_back(1);
	num.push_back(2);
	num.push_back(7);
	num.push_back(6);
	num.push_back(1);
	num.push_back(5);
	vector<vector<int>> result = s.subet(num, 8);
	for (int i = 0; i < result.size(); i++)
	{
		for (int j = 0; j < result[i].size(); j++)
		{
			cout << "[" << result[i][j] << "]" << " ";
		}
		cout << endl;
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
