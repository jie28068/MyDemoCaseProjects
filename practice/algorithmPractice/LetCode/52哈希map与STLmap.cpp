#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <map>
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:map����//��һ��ֵΪ��ֵ���ڶ���Ϊʵֵ ӳ������
 */

struct ListNode
{
	string key;
	int value;
	ListNode* next;
	ListNode(int x):value(x), next(NULL) {}
};
int main()
{
	map<string, int> hash_map;
	string str1 = "ac";
	string str2 = "aaaaaaa";
	string str3 = "23124235";

	hash_map[str1] = 1;
	hash_map[str2] = 50;
	hash_map[str3] = 100;
	if (hash_map.find(str1) != hash_map.end())
	{
		printf("%s �ڹ�ϣ���У���ֵΪ%d\n", str1.c_str(), hash_map[str1]);
	}

	map<string, int> ::iterator it;
	for ( it = hash_map.begin(); it != hash_map.end(); it++)
	{
		printf("hash_map[%s] = %d\n", it->first.c_str(), it->second);
	}
	system("pause");
	return  EXIT_SUCCESS;
}

#endif
