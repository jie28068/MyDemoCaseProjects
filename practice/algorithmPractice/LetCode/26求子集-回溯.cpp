#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <iostream>
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
	vector<vector<int>> subet(vector<int>& num)
	{
		/*存放子集*/
		vector<int> item;
		/*存放结果*/
		vector<vector<int>> result;
		/*递归*/
		generate(0, num, item, result);
		return result;
	}

private:
	void generate(int i, vector<int>& num, vector<int>& item, vector<vector<int>>& result)
	{
		/*结束条件*/
		if (i >= num.size())
		{
			return;
		}
		/*往子集中加数据*/
		item.push_back(num[i]);
		/*往总集中加数据*/
		result.push_back(item);
		/*递归*/
		generate(i + 1, num, item, result);
		/*回溯*/
		item.pop_back();
		/*再次递归*/
		generate(i + 1, num, item, result);
	}
};

int main()
{
	solution s;
	vector<int> num;
	num.push_back(1);
	num.push_back(2);
	num.push_back(3);
	num.push_back(4);
	vector<vector<int>> result = s.subet(num);
	for (int i = 0; i < result.size(); i++)
	{
		for (int j = 0; j < result[i].size(); j++)
		{
			cout << "[" << result[i][j]<<"]"<<" ";
		}
		cout << endl;
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
