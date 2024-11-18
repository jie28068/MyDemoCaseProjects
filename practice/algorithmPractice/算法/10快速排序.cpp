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

void func(vector<int>& arr, int begin, int end)
{
	if (begin < end)
	{
		//“左指针”
		int i = begin;
		//“右指针”
		int j = end;
		//以第一个数作为基数
		int temp = arr[begin];
		while (i < j)
		{
			while (i < j && arr[j] > temp)
			{
				j--;
			}
			arr[i] = arr[j];

			while (i < j && arr[i] <= temp)
			{
				i++;
			}
			arr[j] = arr[i];
		}
		arr[i] = temp;
		func(arr, begin, i- 1);
		func(arr, i + 1, end);
	}
	else
		return;
}

int main()
{
	vector<int> arr{ 65,45,8,3,95,41,25,7,45,15 };
	func(arr,0,9);
	for (int i = 0; i < arr.size();)
	{
		cout << arr[i++] << endl;
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
