#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:打气球,求最少要的射手数
   * Explain:vector中的pair特性,排好序不断更新交集，没有交集就增加射手
 */

/**/
bool cmp(pair<int, int>& a, pair<int, int>& b)
{
	return a.first < b.first;
}
//

class func
{
public:
	int change(std::vector<std::pair<int, int>>& num)
	{
		/*数据为空直接返回*/
		if (num.size() < 0)
		{
			return 0;
		}

		/*排序*/
		sort(num.begin(), num.end(),cmp);

		/*初始化*/
		int number = 1;
		int short_begin = num[0].first;
		int short_end = num[0].second;

		/*循环*/
		for (int i = 1; i < num.size(); i++)
		{
			/*如果有重叠一直交换*/
			if (short_end >= num[i].first)
			{
				short_begin = num[i].first;
				if (short_end > num[i].second)
				{
					short_end = num[i].second;
				}
			}
			else
			{
				/*增加射手*/
				number++;
				short_begin = num[i].first;
				short_end = num[i].second;
			}
		}
		return number;
	}
};
int main()
{
	vector<pair<int, int>>num;
	num.push_back(make_pair(10, 16));
	num.push_back(make_pair(1, 6));
	num.push_back(make_pair(2, 8));
	num.push_back(make_pair(7, 12));
	func f;
	cout<<f.change(num)<<endl;

	system("pause");
	return  EXIT_SUCCESS;
}
#endif
