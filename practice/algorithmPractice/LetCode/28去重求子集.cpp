#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
class solution
{
public:
	vector<vector<int >> subest(vector<int >& num)
	{
		/*存放子集*/
		vector<int> item;
		/*存放结果*/
		vector<vector<int>> result;
		/*去重集合*/
		set<vector<int>> ret;
		/*排序*/
		sort(num.begin(), num.end());

		//result.push_back(item);
		generate(0, num, item, result, ret);

		return result;
	}

private:
	void generate(int i, vector<int>& num, vector<int> &item, vector<vector<int>>& result, set<vector<int>>& ret)
	{
		if (i >= num.size())
		{
			return;
		}
		item.push_back(num[i]);
		if (ret.find(item) == ret.end())
		{
			result.push_back(item);
			ret.insert(item);
		}
		generate(i + 1, num, item, result, ret);
		item.pop_back();
		generate(i + 1, num, item, result, ret);
	}
};
int main()
{
	solution s;
	vector<int> num;
	num.push_back(2);
	num.push_back(1);
	num.push_back(2);
	num.push_back(2);
	vector<vector<int>> result = s.subest(num);
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
