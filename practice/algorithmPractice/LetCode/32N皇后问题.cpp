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
	/*实现函数，返回值为最终的结果*/
	vector<vector<string>> func(int n)
	{
		/*结果容器*/
		vector<vector<string>> result;
		/*保存改行内容的容器*/
		vector<string> item;
		/*存放标志位的容器*/
		vector<vector<int>> mark;
		/*初始化容器(标志，行)*/
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
		/*递归调用*/
		gen(0, n, result, mark, item);
		/*返回值*/
		return result;
	}

	/*递归函数*/
	void gen(int k, int n, vector<vector<string>>& result, vector<vector<int>>& mark, vector<string>& item)
	{
		/*结束条件，k=n*/
		if (k == n)
		{
			result.push_back(item);
			return;
		}
		/*循环行中标志*/
		for (int i = 0; i < n; i++)
		{
			/*判断是否能放皇后*/
			if (mark[k][i] == 0)
			{
				/*记录当前标志容器，方便回溯*/
				vector<vector<int>> t_mark = mark;

				/*标记位皇后*/
				item[k][i] = '*';

				/*更新标志容器*/
				put_xy(k, i, mark);
				/*递归*/
				gen(k + 1, n, result, mark, item);

				/*回溯*/
				mark = t_mark;

				/*重置皇后*/
				item[k][i] = 'O';
			}
		}
	}
	/*标志位二维数组生成函数*/

	void put_xy(int x, int y, vector<vector<int>>& mark)
	{
		/*方向数组*/
		static const int dx[] = { -1,1,0,0,-1,1,-1,1 };
		static const int dy[] = { 0,0,1,-1,1,-1,-1,1 };
		/*传入坐标标记*/
		mark[x][y] = 1;
		/*循环八个方向*/
		for (int i = 1; i < mark.size(); i++)
		{
			/*八个方向及其衍生都标记*/
			for (int j = 0; j < 8; j++)
			{
				int new_x = x + i * dx[j];
				int new_y = y + i * dy[j];

				/*没有越界*/
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
