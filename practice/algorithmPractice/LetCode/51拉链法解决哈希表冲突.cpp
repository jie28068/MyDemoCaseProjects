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

/*结构体*/
struct ListNode
{
	int value;
	ListNode* next;
	ListNode(int x):value(x),next(NULL){}
};

/*直接对整数取余表长再返回*/
int int_func(int key, int table)
{
	return key % table;
}

/*插入哈希表
 *参数一：哈希表
 *参数二：要插入的数值
 *参数三：表长*/
void insert(ListNode* hash_table[], ListNode* node, int table_len)
{
	/*hash关键字*/
	int hash_key = int_func(node->value, table_len);
	
	///*头插法插入节点*/
	node->next = hash_table[hash_key];
	hash_table[hash_key] = node;


}

/*查找哈希表中的值*/
bool search(ListNode* hash_table[], int val, int table_len)
{
	int hash_key = int_func(val, table_len);

	ListNode* node = hash_table[hash_key];
	while (node)
	{
		if (node->value == val)
		{
			return true;
		}
		node = node->next;
	}
	return false;
}
int main()
{
	/*取质数冲突会变少*/
	const int TABLE_LEN = 11;

	ListNode* hash_table[TABLE_LEN] = { 0 };
	vector<ListNode*> vec;
	int text[] = { 1,1,4,9,20,60,500,1599 };

	/*数组放置到容器中*/
	for (int i = 0; i < 8; i++)
	{
		vec.push_back(new ListNode(text[i]));
	}

	/*插入哈希表*/
	for (int i = 0; i < vec.size(); i++)
	{
		insert(hash_table, vec[i], TABLE_LEN);
	}

	for (int i = 0; i < TABLE_LEN; i++)
	{
		printf("[%d]:", i);
		ListNode* head = hash_table[i];
		while (head)
		{
			printf("->%d", head->value);
			head = head->next;
		}
		printf("\n");
	}
	printf("\n");

	for (int i = 0; i < 2000; i++)
	{
		if (search(hash_table, i, TABLE_LEN))
		{
			printf("%d是数组中的值\n",i);
		}
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
