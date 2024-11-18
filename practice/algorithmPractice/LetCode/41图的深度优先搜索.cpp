#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:图的遍历要设置标志位数组，附近的点用容器存放，深度一般与递归有关
 */

/*结构体*/
struct grapnode
{
	int value;
	const char* data;
	vector<grapnode*> neighbors;
	grapnode(const char* x,int y) :data(x),value(y) {};
};

void DFS_grap(grapnode* node, int visit[])
{
	/*标记以访问的点*/
	visit[node->value] = 1;
	printf("%s ", node->data);

	/*访问相邻且没有被访问的点*/
	for (int i = 0; i < node->neighbors.size(); i++)
	{
		if (visit[node->neighbors[i]->value] == 0)
		{
			DFS_grap(node->neighbors[i], visit);
		}
	}
}
int main()
{
	const int max = 5;
	grapnode* ga[max];

	ga[0] = new grapnode("吴军",0);
	ga[1] = new grapnode("周泳",1);
	ga[2] = new grapnode("黄赞",2);
	ga[3] = new grapnode("刘黄鑫",3);
	ga[4] = new grapnode("龙浩华",4);


	ga[0]->neighbors.push_back(ga[4]);
	ga[0]->neighbors.push_back(ga[2]);
	ga[1]->neighbors.push_back(ga[0]);
	ga[1]->neighbors.push_back(ga[2]);
	ga[2]->neighbors.push_back(ga[3]);
	ga[3]->neighbors.push_back(ga[4]);
	ga[4]->neighbors.push_back(ga[3]);

	int visit[max] = { 0 };
	for (int i = 0; i < max; i++)
	{
		if (visit[i] == 0)
		{
			printf("与%s点相邻的是：", ga[i]->data);
			DFS_grap(ga[i], visit);
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