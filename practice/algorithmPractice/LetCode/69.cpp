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
	//��ȡ�仯�����out...��
	for (int i = 1; i < n; ++i)
	{
		outputIndexArray[i-1] = abs(inputArray[i-1] - inputArray[i]);
	}
	//�������ֵ����
	for (int i = 0; i < n - 1; ++i)
	{
		cout << outputIndexArray[i] << endl;
	}

	//ð������
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

	//�������ú�ľ���ֵ����
	for (int i = 0; i < n -1; ++i)
	{
		cout << outputIndexArray[i] << endl;
	}

	cout << "****************" << endl;
	//���ǰM��
	for (int i = 1; i < m; ++i)
	{
		cout << outputIndexArray[i] << endl;
	}

	//����outputIndexArray����
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
