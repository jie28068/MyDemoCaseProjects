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
		/*����Ӽ�*/
		vector<int> item;
		/*��Ž��*/
		vector<vector<int>> result;
		/*�ݹ�*/
		generate(0, num, item, result);
		return result;
	}

private:
	void generate(int i, vector<int>& num, vector<int>& item, vector<vector<int>>& result)
	{
		/*��������*/
		if (i >= num.size())
		{
			return;
		}
		/*���Ӽ��м�����*/
		item.push_back(num[i]);
		/*���ܼ��м�����*/
		result.push_back(item);
		/*�ݹ�*/
		generate(i + 1, num, item, result);
		/*����*/
		item.pop_back();
		/*�ٴεݹ�*/
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
