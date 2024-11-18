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

typedef char VElemtype;//����Ԫ�ص�����
typedef int adjType;    //����Ȩ������

typedef struct MyGragh
{
    VElemtype Vertex[VMAX];//���㼯��V
    adjType A[VMAX][VMAX];//�ٽ���� "��"
    int VexNum;//ͼ����Ч�Ķ�����;
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

    //��Ч�����ʼ��
    G->VexNum = 0;  

    //���㼯�ϳ�ʼ��
    for (i = 0; i < VMAX; i++)
    {
        G->Vertex[i] = 0;
    }

    //�����ʼ��
    for (i = 0; i < VMAX; i++)
    {
        for (j = 0; j < VMAX; j++)
        {
            G->A[i][j] = INT_MAX;
        }
    }

    //1.���㼯��
    cout << "���㼯�ϣ�#����" << endl;
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
    //��ӡ���� ���һ��
    for (int i = 0; i < G->VexNum; i++)
    {
        cout<< G->Vertex[i]<<endl;
    } 


    // 2.��ϵ����
   // ����ߵĹ�ϵ ��� �յ� ��㵽�յ��Ȩֵ
   cout<<"�������ϵ���ϣ� ��� �յ� ��㵽�յ��Ȩֵ"<<endl;
    VElemtype startData, endData;
    int w;
    int StratIndex, endIndex;
    while (1)
    {
        getchar();//�ѻس�������
        cin>>startData>>endData>>w;
        if (startData == '#')
        {
            break;
        }
        StratIndex = GetVexIndex(G, startData);
        endIndex = GetVexIndex(G, endData);
        if (StratIndex == -1 || endIndex == -1)
        {
           cout<<"������������յ��д� ����������"<<endl;
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


