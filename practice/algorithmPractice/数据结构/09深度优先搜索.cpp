#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
using namespace std;
#define MAX 6
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:DFS    �ݹ�
 */

//ͼ����Ӿ���
struct map
{
	//������
	int data;   

	//ָ����
	vector<map*> m_next;

	//���캯��
	map(int x) :data(x), m_next(NULL) {};
};

void DFS_map(map* node, int* vist)
{
	//����Է��ʵĵ�
	vist[node->data] = 1;

	//�����ǰ��
	cout << node->data;

	//����������û�б����ʵĵ�
	for (int i = 0; i < node->m_next.size(); i++)
	{
		if (vist[node->m_next[i]->data] == 0)
		{
			DFS_map(node->m_next[i], vist);
		}
	}
}

int main()
{
	//��ǵ��ʼ��
	int visit[MAX] = { 0 };

	//ͼ�ĳ�ʼ��
	map* mp[MAX];
	for (int i = 0; i < MAX; i++)
	{
		mp[i] = new map(i);
	}

	//ͼ�Ĺ�ϵ
	mp[0]->m_next.push_back(mp[2]);
	mp[0]->m_next.push_back(mp[4]);
	mp[1]->m_next.push_back(mp[0]);
	mp[1]->m_next.push_back(mp[2]);
	mp[2]->m_next.push_back(mp[3]);
	mp[3]->m_next.push_back(mp[4]);
	mp[4]->m_next.push_back(mp[3]);

	//����
	for (int i = 0; i < MAX; i++)
	{
		//��ǵ�Ϊ��Żᱻ����
		if (visit[i] == 0)
		{
			cout << "��" << mp[i]->data << "Ϊ�ڵ������������";
			DFS_map(mp[i], visit);
			cout << endl;
		}
	}

	//���ͷ�
	for (int i = 0; i < MAX; i++)
	{
		delete mp[i];
	}

	system("pause");
	return  EXIT_SUCCESS;
}

#endif;