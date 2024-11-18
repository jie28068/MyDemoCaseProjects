#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:巧妙
 */
class solution
{
public:
	vector<vector<int>> subet(vector<int>& num)
	{
		vector<vector<int>> result;
		/*得到子集个数*/
		int all_size = 1 << num.size();
		for (int i = 0; i < all_size; i++)
		{
			vector<int> item;
			for (int j = 0; j < num.size(); j++)
			{
				if (i & (1 << j))
				{
					item.push_back(num[j]);
				}
			}
			result.push_back(item);
		}
		return result;
	}
};
int main()
{
	solution s;
	vector<int> num;
	num.push_back(71);
	num.push_back(5);
	num.push_back(96);
	num.push_back(1);
	vector<vector<int>> result = s.subet(num);
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
