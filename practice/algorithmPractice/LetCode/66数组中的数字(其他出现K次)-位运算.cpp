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

int func(vector<int>& arr, int k)
{
	int result = 0;
	for (int i = 0; i < 32; ++i)
	{
		int sum = 0;
		for (auto j : arr)
		{
			//得到该位上有几个1，通过右移与1
			sum +=(j >> i) & 1;
		}
		//该位上的1可以对k取余，就是0，反之就是1，再把它左移回去
		result +=(sum % k) << i;
	}
	return result;
}
int main()
{
	vector<int> arr;
	arr.push_back(5);
	arr.push_back(1);
	arr.push_back(5);
	arr.push_back(5);
	arr.push_back(4);
	arr.push_back(1);
	arr.push_back(1);
	int a = func(arr, 3);
	cout << a << endl;
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
