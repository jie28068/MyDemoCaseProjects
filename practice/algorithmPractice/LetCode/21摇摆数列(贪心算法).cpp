#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:有限自动状态机,求一个摇摆数列的最大子数列的元素个数,极值
 */
#include <vector>
class func
{
public:
	int max_func(vector<int> &num)
	{
		/*三种状态*/
		const int open_star = 0;
		const int up_star = 1;
		const int down_star = 2;
		int STATAR = open_star;
		int length = 0;  //初始化长度
		
		/*只有一个元素时*/
		if (num.size() < 2)
		{
			return num.size();
		}

		/*遍历*/
		for (int i = 1; i < num.size(); i++)
		{

			switch (STATAR)
			{
				/*开始状态*/
			case open_star:
				if(num[i-1] < num[i])
				{
					STATAR = up_star;
					length++;
				}
				else if (num[i - 1] > num[i])
				{
					STATAR = down_star;
					length++;
				}				
				break;

				/*上升状态*/
			case up_star:
				if (num[i - 1] > num[i])
				{
					STATAR = down_star;
					length++;
				}
				break;

				/*下降状态*/
			case down_star:
				if (num[i - 1] < num[i])
				{
					STATAR = up_star;
					length++;
				}
				break;
			}

		}
		return length;
	}

};


int main()
{
	func f;
	vector<int> num;
	num.push_back(1);
	num.push_back(17);
	num.push_back(5);
	num.push_back(10);
	num.push_back(13);
	num.push_back(15);
	num.push_back(10);
	num.push_back(5);
	num.push_back(16);
	num.push_back(8);
	cout << f.max_func(num) << endl;

	system("pause");
	return  EXIT_SUCCESS;
}
#endif
