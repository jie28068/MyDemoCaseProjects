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
void func(vector<int>& arr)
{
	for (int i = 0; i < arr.size(); ++i)
	{
		for (int j = 0; j < arr.size() - i - 1; ++j)
		{
			if (arr[j] > arr[j + 1])
			{
				arr[j] = arr[j] ^ arr[j + 1];
				arr[j+1] = arr[j] ^ arr[j + 1];
				arr[j] = arr[j] ^ arr[j + 1];
			}
		}
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
