#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
/*
	* User:SJ
   * Time:
   * Function:
   * Explain:�������飬Ҫ������һ������������һ�����飬������һ������С�����ж��ٸ�
 */

/*��*/
class func
{
public:
	/*ʵ��*/
	int realize(vector<int>&, vector<int>&);
};

int func::realize(vector<int>& num1, vector<int>& num2)
{
	/*���ź���*/
	sort(num1.begin(), num1.end());
	sort(num2.begin(), num2.end());

	int child = 0;    //����
	int cooker = 0;   //�ǹ�
	/**/
	while (child < num1.size() && cooker < num2.size())
	{
		if (num1[child] < num2[cooker])
		{
			child++;
		}
		cooker++;
	}
	return child;
}

int main()
{
	func f;
	vector<int>num1;
	vector<int>num2;
	num1.push_back(5);
	num1.push_back(10);
	num1.push_back(2);
	num1.push_back(9);
	num1.push_back(15);
	num1.push_back(9);
	num2.push_back(6);
	num2.push_back(1);
	num2.push_back(20);
	num2.push_back(3);
	num2.push_back(8);
	cout<<f.realize(num1, num2)<<endl;
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
