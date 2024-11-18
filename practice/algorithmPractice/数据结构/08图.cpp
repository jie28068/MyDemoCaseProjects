#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
#define VMAX 100
#define INT_MAX 65535
#define TRUE    1
#define FALSE   0 

typedef char VElemtype;//顶点元素的类型
typedef int adjType;    //边上权的类型

typedef struct MyGragh
{
    VElemtype Vertex[VMAX];//顶点集合V
    adjType A[VMAX][VMAX];//临界矩阵 "边"
    int VexNum;//图中有效的顶点数;
}MyGragh;

class Test_Tu
{
public:
    void myVisit(MyGragh* G, int v0);
    static int GetVexIndex(MyGragh* G, VElemtype v0);
    MyGragh* CreateGraph();
};

void Test_Tu::myVisit(MyGragh* G, int v0)
{
    //if (sizeof(VElemtype) == sizeof(int))
    //{
    //    printf("%d ", G->Vertex[v0]);
    //}
    //else if (sizeof(VElemtype) == sizeof(char))
    //{
    //    printf("%c ", G->Vertex[v0]);
    //}

}

int Test_Tu::GetVexIndex(MyGragh* G, VElemtype v0)
{
    for (int i = 0; i < G->VexNum; i++)
    {
        if (G->Vertex[i] == v0)
        {
            return i;
        }
    }
    return -1;
}

MyGragh* Test_Tu::CreateGraph()
{
    int i, j;
    VElemtype data;
    MyGragh* G = new MyGragh;

    //有效顶点初始化
    G->VexNum = 0;  

    //顶点集合初始化
    for (i = 0; i < VMAX; i++)
    {
        G->Vertex[i] = 0;
    }

    //矩阵初始化
    for (i = 0; i < VMAX; i++)
    {
        for (j = 0; j < VMAX; j++)
        {
            G->A[i][j] = INT_MAX;
        }
    }

    //1.顶点集合
    cout << "顶点集合，#结束" << endl;
    for (i = 0; 1; i++)
    {
        cin >> data;
        if (data == '#' && G->VexNum < VMAX)
        {
            break;
        }
        G->Vertex[i] = data;
        G->VexNum++;
    }
    //打印出来 检查一下
    for (int i = 0; i < G->VexNum; i++)
    {
        cout<< G->Vertex[i]<<endl;
    } 


    // 2.关系集合
   // 输入边的关系 起点 终点 起点到终点的权值
   cout<<"请输入关系集合： 起点 终点 起点到终点的权值"<<endl;
    VElemtype startData, endData;
    int w;
    int StratIndex, endIndex;
    while (1)
    {
        getchar();//把回车读出来
        cin>>startData>>endData>>w;
        if (startData == '#')
        {
            break;
        }
        StratIndex = GetVexIndex(G, startData);
        endIndex = GetVexIndex(G, endData);
        if (StratIndex == -1 || endIndex == -1)
        {
           cout<<"你输入的起点或终点有错 请重新输入"<<endl;
            continue;
        }
        G->A[StratIndex][endIndex] = w;
    }
    for (i = 0; i < G->VexNum; i++)
    {
        for (j = 0; j < G->VexNum; j++)
        {
           cout<<"\t"<< G->A[i][j];
        }
        cout << endl;
    }
    return G;
}

int main()
{
    Test_Tu t;
    t.CreateGraph();

	system("pause");
	return  EXIT_SUCCESS;
}
#endif;


