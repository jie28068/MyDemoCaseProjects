#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:ӳ�䣬�ؼ���
   * Explain:T = O(n)
 */

/*��������*/
void func()
{
	int random[10] = { 999,1,444,7,20,9,1,3,7,7 };
	int hash_map[1000] = { 0 };

	/*��ϣ�����������ֳ��ֵĴ���*/
	for (int i = 0; i < 10; i++)
	{
		hash_map[random[i]]++;
	}

	for (int i = 0; i < 1000; i++)
	{
		/*ֻ�й�ϣ���г��ֵ����ֲŻ���뵽�ò�ѭ��*/
		for (int j = 0; j < hash_map[i]; j++)
		{
			cout << i << endl;
		}
	}
}

/*ֱ�Ӷ�����ȡ����ٷ���*/
int int_func(int key, int table)
{
	return key % table;
}

/*���ַ����е��ַ���ASC����ӵõ�������ȡ���*/
int string_func(string key, int table)
{
	int sum = 0;
	for (int i = 0; i < key.length(); i++)
	{
		sum += key[i];
	}
	return sum % table;
}
int main()
{
	func();
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
