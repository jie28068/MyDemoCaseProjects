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
   * Explain:DFS    递归
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

void DFS_map(map* node, int* vist)
{
	//标记以访问的点
	vist[node->data] = 1;

	//输出当前点
	cout << node->data;

	//访问相邻且没有被访问的点
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
			DFS_map(mp[i], visit);
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