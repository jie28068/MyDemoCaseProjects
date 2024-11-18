#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <queue>
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:宽度搜索一般都与队列有关
 */
/**/
struct grapnode
{
	int value;
	const char* data;
	vector<grapnode*> night;
	grapnode(const char* x, int y) :data(x), value(y) {};
};

void BFS_grap(grapnode* node, int visit[])
{
	/*队列*/
	queue<grapnode*> qu;
	qu.push(node);
	visit[node->value] = 1;
	while (!qu.empty())
	{
		grapnode* pnode = qu.front();
		qu.pop();
		printf("%s ", pnode->data);
		for (int i = 0; i < node->night.size(); i++)
		{
			if (visit[node->night[i]->value] == 0)
			{
				qu.push(node->night[i]);
				visit[node->night[i]->value] = 1;
			}
		}
	}
}
int main()
{
	const int max = 5;
	grapnode* ga[max];

	ga[0] = new grapnode("吴军", 0);
	ga[1] = new grapnode("周泳", 1);
	ga[2] = new grapnode("黄赞", 2);
	ga[3] = new grapnode("刘黄鑫", 3);
	ga[4] = new grapnode("龙浩华", 4);


	ga[0]->night.push_back(ga[4]);
	ga[0]->night.push_back(ga[2]);
	ga[1]->night.push_back(ga[0]);
	ga[1]->night.push_back(ga[2]);
	ga[2]->night.push_back(ga[3]);
	ga[3]->night.push_back(ga[4]);
	ga[4]->night.push_back(ga[3]);

	int visit[max] = { 0 };
	for (int i = 0; i < max; i++)
	{
		if (visit[i] == 0)
		{
			printf("与%s点相邻的是：", ga[i]->data);
			BFS_grap(ga[i], visit);
			printf("\n");
		}
	}
	for (int i = 0; i < max; i++)
	{
		delete ga[i];
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
