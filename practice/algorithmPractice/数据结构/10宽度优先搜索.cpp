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
   * Explain:BFS      队列
 */
 //图的领接矩阵
struct map
{
	//数据域
	int data;

	//指针域
	vector<map*> m_next;

	//构造函数
	map(int x) :data(x), m_next(NULL) {};
};

void BFS_map(map* node, int* vist)
{
	queue<map*> Q;

	//往队列容器里面加节点
	Q.push(node);

	//访问过的置一
	vist[node->data] = 1;

	//队列不为空就一直循环
	while (!Q.empty())
	{
		//获取队列中的第一个节点并抛出
		map* node = Q.front();
		Q.pop();

		//打印
		cout << node->data;

		//访问相邻且没有被访问的点
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
	//标记点初始化
	int visit[MAX] = { 0 };

	//图的初始化
	map* mp[MAX];
	for (int i = 0; i < MAX; i++)
	{
		mp[i] = new map(i);
	}

	//图的关系
	mp[0]->m_next.push_back(mp[2]);
	mp[0]->m_next.push_back(mp[4]);
	mp[1]->m_next.push_back(mp[0]);
	mp[1]->m_next.push_back(mp[2]);
	mp[2]->m_next.push_back(mp[3]);
	mp[3]->m_next.push_back(mp[4]);
	mp[4]->m_next.push_back(mp[3]);

	//遍历
	for (int i = 0; i < MAX; i++)
	{
		//标记点为零才会被访问
		if (visit[i] == 0)
		{
			cout << "以" << mp[i]->data << "为节点深度搜索到：";
			BFS_map(mp[i], visit);
			cout << endl;
		}
	}

	//堆释放
	for (int i = 0; i < MAX; i++)
	{
		delete mp[i];
	}

	system("pause");
	return  EXIT_SUCCESS;
}
#endif;
