#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */

void func(vector<int>& arr)
{
	for (int i = 0; i < arr.size(); ++i)
	{
		int temp = i;
		for (int j = i + 1; j < arr.size(); ++j)
		{
			if (arr[j] < arr[temp])
			{
				temp = j;
			}
		}
		int tmp = arr[temp];
		arr[temp] = arr[i];
		arr[i] = tmp;
	}
}
int main()
{
	vector<int> arr{ 65,45,8,3,95,41,25,7,45,15 };
	func(arr);
	for (int i = 0; i < arr.size();)
	{
		cout << arr[i++] << endl;
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
