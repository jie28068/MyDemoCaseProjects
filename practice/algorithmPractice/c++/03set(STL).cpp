#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <set> //set����
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
int main()
{
	std::set<int> test; //��������
	const int A = 7;
	const int B = 8;
	int a[A] = { 4,1,5,8,10,1,3 };
	int b[B] = { 2,7,6,3,1,6,0,1 };

	for (int i = 0; i < A; i++)
	{
		test.insert(a[i]);//������a����������

	}
	for (int i = 0; i < B; i++)
	{
		if (test.find(b[i]) != test.end())//������������û�к�b������ͬ��ֵ
		{
			cout <<"�ڼ�����"<< i << endl<<"��ֵͬ��" << b[i] << endl;
		}
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif;

