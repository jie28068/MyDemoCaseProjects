#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:ͼ�ı���Ҫ���ñ�־λ���飬�����ĵ���������ţ����һ����ݹ��й�
 */

/*�ṹ��*/
struct grapnode
{
	int value;
	const char* data;
	vector<grapnode*> neighbors;
	grapnode(const char* x,int y) :data(x),value(y) {};
};

void DFS_grap(grapnode* node, int visit[])
{
	/*����Է��ʵĵ�*/
	visit[node->value] = 1;
	printf("%s ", node->data);

	/*����������û�б����ʵĵ�*/
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

	ga[0] = new grapnode("���",0);
	ga[1] = new grapnode("��Ӿ",1);
	ga[2] = new grapnode("����",2);
	ga[3] = new grapnode("������",3);
	ga[4] = new grapnode("���ƻ�",4);


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
			printf("��%s�����ڵ��ǣ�", ga[i]->data);
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