#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:������,������Ҫ��������
   * Explain:vector�е�pair����,�ź��򲻶ϸ��½�����û�н�������������
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
		/*����Ϊ��ֱ�ӷ���*/
		if (num.size() < 0)
		{
			return 0;
		}

		/*����*/
		sort(num.begin(), num.end(),cmp);

		/*��ʼ��*/
		int number = 1;
		int short_begin = num[0].first;
		int short_end = num[0].second;

		/*ѭ��*/
		for (int i = 1; i < num.size(); i++)
		{
			/*������ص�һֱ����*/
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
				/*��������*/
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
