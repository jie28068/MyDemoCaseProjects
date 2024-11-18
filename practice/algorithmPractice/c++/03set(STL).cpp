#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <set> //set容器
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
int main()
{
	std::set<int> test; //定义容器
	const int A = 7;
	const int B = 8;
	int a[A] = { 4,1,5,8,10,1,3 };
	int b[B] = { 2,7,6,3,1,6,0,1 };

	for (int i = 0; i < A; i++)
	{
		test.insert(a[i]);//将数组a插入容器中

	}
	for (int i = 0; i < B; i++)
	{
		if (test.find(b[i]) != test.end())//在容器中找有没有和b数组相同的值
		{
			cout <<"第几个："<< i << endl<<"相同值：" << b[i] << endl;
		}
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif;

