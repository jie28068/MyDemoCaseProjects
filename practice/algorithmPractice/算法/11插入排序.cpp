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
void func(int* arr)
{
	int temp =0, j =0;
	for (int i = 1; i < 10 ; ++i)
	{
		 temp = arr[i];
		 j = i - 1;
		while (j >= 0 && temp < arr[j])
		{
			arr[j + 1] = arr[j--];
		}
		arr[j + 1] = temp;
	}
}

int main()
{
	int arr[] = { 65,45,8,3,95,41,25,7,45,15 };
	func(arr);
	for (int i = 0; i < 10;)
	{
		cout << arr[i++] << endl;
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
