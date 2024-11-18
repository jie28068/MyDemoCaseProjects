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

/*�ṹ��*/
struct ListNode
{
	int value;
	ListNode* next;
	ListNode(int x):value(x),next(NULL){}
};

/*ֱ�Ӷ�����ȡ����ٷ���*/
int int_func(int key, int table)
{
	return key % table;
}

/*�����ϣ��
 *����һ����ϣ��
 *��������Ҫ�������ֵ
 *����������*/
void insert(ListNode* hash_table[], ListNode* node, int table_len)
{
	/*hash�ؼ���*/
	int hash_key = int_func(node->value, table_len);
	
	///*ͷ�巨����ڵ�*/
	node->next = hash_table[hash_key];
	hash_table[hash_key] = node;


}

/*���ҹ�ϣ���е�ֵ*/
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
	/*ȡ������ͻ�����*/
	const int TABLE_LEN = 11;

	ListNode* hash_table[TABLE_LEN] = { 0 };
	vector<ListNode*> vec;
	int text[] = { 1,1,4,9,20,60,500,1599 };

	/*������õ�������*/
	for (int i = 0; i < 8; i++)
	{
		vec.push_back(new ListNode(text[i]));
	}

	/*�����ϣ��*/
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
			printf("%d�������е�ֵ\n",i);
		}
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
