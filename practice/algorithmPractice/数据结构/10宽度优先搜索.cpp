#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#define MAX 6
#include <queue>
#include <vector>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:BFS      ����
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

void BFS_map(map* node, int* vist)
{
	queue<map*> Q;

	//��������������ӽڵ�
	Q.push(node);

	//���ʹ�����һ
	vist[node->data] = 1;

	//���в�Ϊ�վ�һֱѭ��
	while (!Q.empty())
	{
		//��ȡ�����еĵ�һ���ڵ㲢�׳�
		map* node = Q.front();
		Q.pop();

		//��ӡ
		cout << node->data;

		//����������û�б����ʵĵ�
		for (int i = 0; i < node->m_next.size(); i++)
		{
			if (vist[node->m_next[i]->data] == 0)
			{
				Q.push(node->m_next[i]);
				vist[node->m_next[i]->data] = 1;
			}
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
			BFS_map(mp[i], visit);
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
