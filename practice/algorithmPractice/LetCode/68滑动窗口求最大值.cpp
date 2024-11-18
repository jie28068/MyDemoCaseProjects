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
#include <vector>


class Solution
{
public:
	int emp(vector<int>& v, int i, int j)
	{
		int result = v[i];
		for (int i1 = i; i1 < j; i1++)
		{
			if (v[i1] > result)
			{
				result = v[i1];
			}
		}
		return result;
	}

	vector<int> func(vector<int>& v, int k)
	{
		vector<int> vs;
		for (int i = 0; i < v.size(); i++)
		{
			if ((i + k) > (int)v.size())
			{
				break;
			}
			int sum = 0;
			sum = emp(v, i, i + k);
			vs.push_back(sum);
		}
		return vs;
	}
};
int main()
{
	vector<int> v1;
	v1.push_back(1);
	v1.push_back(3);
	v1.push_back(-1);
	v1.push_back(-3);
	v1.push_back(5);
	v1.push_back(3);
	v1.push_back(6);
	v1.push_back(7);
	Solution s;
	vector<int> v2;
	//v2 = s.func(v1, 3);
	int i = 0;
	while (!v2.empty())
	{
		cout << v2[i++] << endl;
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
