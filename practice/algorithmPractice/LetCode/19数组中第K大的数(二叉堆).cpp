#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:��֪δ��������飬���K�������
   * ά��һ��K��С����С�ѣ�������Ԫ��С��Kʱֱ����ѣ����Ѷ�Ԫ��С���½�Ԫ�أ������Ѷ�
   * �Ѷ����ǵ�K���Ԫ��
   * 
 */
#include <queue>
class Func
{
public:
	int find_K(vector<int>& num, int K)
	{
		/*������С��*/
		priority_queue<int, vector<int>, greater<int>> Q;
		for (int i = 0; i < num.size(); i++)
		{
			/*�����������С��K,ֱ�����*/
			if (Q.size() < K)
			{
				Q.push(num[i]);
			}
			/*ά���Ѷ�Ԫ��*/
			else if (Q.top() < num[i])
			{
				Q.pop();
				Q.push(num[i]);
			}
		}
		return Q.top();
	}
};

int main()
{
	Func f;
	vector<int> num;
	num.push_back(3);
	num.push_back(2);
	num.push_back(1);
	num.push_back(5);
	num.push_back(6);
	num.push_back(4);
	cout<<f.find_K(num, 3)<<endl;
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
