#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:例如：有序数组{ -1,2,20,20,20,20,20,20,90 }   target=20  输出对应的下标[2,7]
   *														target=5  输出对应的下标[-1,-1]
   * Explain:通过二分法找到其中一个目标值，然后左右开弓，利用递归分别找到最左和最右的下标即可
 */
/*再次查找*/
void func(vector<int>& sort_array, vector<int>& result, int mid, int target,int i)
{
	if (sort_array[mid] != target)
	{
		return;
	}
	result[i] = mid;
	if (i == 0)
	{
		func(sort_array, result, mid - 1, target, 0);
	}
	func(sort_array, result, mid + 1, target, 1);
}
class solution
{
public:
	vector<int> search_array(vector<int>& sort_array, int target)
	{
		/**创建一个返回数组*/
		vector<int> result;
		/*初始化*/
		for (int i = 0; i < 2; i++)
		{
			result.push_back(-1);
		}
		int begin = 0;
		int end = sort_array.size() - 1;
		/*二分查找*/
		while (begin <= end)
		{
			int mid = (begin + end) / 2;
			if (target == sort_array[mid])
			{
				/*找到了*/
				func(sort_array, result, mid, target, 0);
				/*跳出*/
				break;
			}
			else if (target < sort_array[mid])
			{
				end = mid - 1;
			}
			else if (target > sort_array[mid])
			{
				begin = mid + 1;
			}
		}	
		return result;
	}
};
int main()
{
	solution s;
	vector<int> sort_array;
	int b = 20;
	int a[] = { -1,2,20,20,20,20,20,20,90 };
	for (int i = 0; i < 9; i++)
	{
		sort_array.push_back(a[i]);
	}
	vector<int> result = s.search_array(sort_array,b);
	cout << "[" << result[0] << "," << result[1] << "]" << endl;
	
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
