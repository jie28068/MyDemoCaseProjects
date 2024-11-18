#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <math.h>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
int getMostJumps(int* inputArray, int n, int* outputIndexArray, int m)
{
	//获取变化存放在out...中
	for (int i = 1; i < n; ++i)
	{
		outputIndexArray[i-1] = abs(inputArray[i-1] - inputArray[i]);
	}
	//输出绝对值数组
	for (int i = 0; i < n - 1; ++i)
	{
		cout << outputIndexArray[i] << endl;
	}

	//冒泡排序
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n - i - 1; ++j)
		{
			if (outputIndexArray[j] < outputIndexArray[j + 1])
			{
				int temp = outputIndexArray[j];
				outputIndexArray[j] = outputIndexArray[j + 1];
				outputIndexArray[j + 1] = temp;
			}
		}
	}

	cout << "****************" << endl;

	//输出排序好后的绝对值数组
	for (int i = 0; i < n -1; ++i)
	{
		cout << outputIndexArray[i] << endl;
	}

	cout << "****************" << endl;
	//输出前M个
	for (int i = 1; i < m; ++i)
	{
		cout << outputIndexArray[i] << endl;
	}

	//计算outputIndexArray长度
	int len = 0;
	for (int i = 0; i < n; ++i)
	{
		if (!outputIndexArray[i])
		{

		}
		else
		{
			++len;
		}
	}
	return m < len ? m:len;
}

int main()
{
	int inputArray[20] =
	{
		5, 10, 18, 9, 12, 18, 9, 20,25,22,15,18,9,20,40,42,30,33,33,55
	};
	int outputIndexArray[20];
	int n = getMostJumps(inputArray, 20, outputIndexArray, 5);
	cout << n << endl;
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
