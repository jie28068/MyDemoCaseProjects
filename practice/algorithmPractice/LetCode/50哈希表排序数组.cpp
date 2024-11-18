#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:映射，关键字
   * Explain:T = O(n)
 */

/*排序数组*/
void func()
{
	int random[10] = { 999,1,444,7,20,9,1,3,7,7 };
	int hash_map[1000] = { 0 };

	/*哈希表标记数组数字出现的次数*/
	for (int i = 0; i < 10; i++)
	{
		hash_map[random[i]]++;
	}

	for (int i = 0; i < 1000; i++)
	{
		/*只有哈希表中出现的数字才会进入到该层循环*/
		for (int j = 0; j < hash_map[i]; j++)
		{
			cout << i << endl;
		}
	}
}

/*直接对整数取余表长再返回*/
int int_func(int key, int table)
{
	return key % table;
}

/*将字符串中的字符的ASC马相加得到整数再取余表长*/
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
