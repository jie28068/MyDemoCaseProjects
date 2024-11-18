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
class soluton
{
public:
	/*ʵ�ֺ���������ֵΪ���յĽ��*/
	vector<vector<string>> func(int n)
	{
		/*�������*/
		vector<vector<string>> result;
		/*����������ݵ�����*/
		vector<string> item;
		/*��ű�־λ������*/
		vector<vector<int>> mark;
		/*��ʼ������(��־����)*/
		for (int i = 0; i < n; i++)
		{
			mark.push_back(vector<int>());
			for (int j = 0; j < n; j++)
			{
				mark[i].push_back(0);
			}
			item.push_back("");
			item[i].append(n, 'O');
		}
		/*�ݹ����*/
		gen(0, n, result, mark, item);
		/*����ֵ*/
		return result;
	}

	/*�ݹ麯��*/
	void gen(int k, int n, vector<vector<string>>& result, vector<vector<int>>& mark, vector<string>& item)
	{
		/*����������k=n*/
		if (k == n)
		{
			result.push_back(item);
			return;
		}
		/*ѭ�����б�־*/
		for (int i = 0; i < n; i++)
		{
			/*�ж��Ƿ��ܷŻʺ�*/
			if (mark[k][i] == 0)
			{
				/*��¼��ǰ��־�������������*/
				vector<vector<int>> t_mark = mark;

				/*���λ�ʺ�*/
				item[k][i] = '*';

				/*���±�־����*/
				put_xy(k, i, mark);
				/*�ݹ�*/
				gen(k + 1, n, result, mark, item);

				/*����*/
				mark = t_mark;

				/*���ûʺ�*/
				item[k][i] = 'O';
			}
		}
	}
	/*��־λ��ά�������ɺ���*/

	void put_xy(int x, int y, vector<vector<int>>& mark)
	{
		/*��������*/
		static const int dx[] = { -1,1,0,0,-1,1,-1,1 };
		static const int dy[] = { 0,0,1,-1,1,-1,-1,1 };
		/*����������*/
		mark[x][y] = 1;
		/*ѭ���˸�����*/
		for (int i = 1; i < mark.size(); i++)
		{
			/*�˸����������������*/
			for (int j = 0; j < 8; j++)
			{
				int new_x = x + i * dx[j];
				int new_y = y + i * dy[j];

				/*û��Խ��*/
				if (new_x >= 0 && new_x < mark.size() && new_y >= 0 && new_y < mark.size())
				{
					mark[new_x][new_y] = 1;
				}
			}
		}
	}

};
int main()
{
	soluton p;
	vector<vector<string>> result;
	result = p.func(4);
	for (int i = 0; i < result.size(); i++)
	{
		printf("i=%d\n", i);
		for (int j = 0; j < result[i].size(); j++)
		{
			printf("%s\n", result[i][j].c_str());
		}
		printf("\n");
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
