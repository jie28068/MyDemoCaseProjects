#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:���Գ�ʼ��-����ʽ
*/

class Solution
{
public:
	/*ɾ�������͸�ֵ�����*/
	Solution(const Solution&) = delete;
	Solution& operator= (const Solution&) = delete;

	/*��̬��ȡ���캯��*/
	static Solution& getgouzao()
	{
		/*��̬����,����ʼ��*/
		static Solution s(0);
		/*��������*/
		return s;
	}

	/*Get*/
	int getvalue()
	{
		return x;
	}

	/*Set*/
	void setvalue(int i)
	{
		x = i;
	}
private:
	int x;
	/*˽�л����캯��*/
	Solution(int i):x(i){}
};
int main()
{
	Solution& s = Solution::getgouzao();

	cout << s.getvalue() << endl;
	s.setvalue(10);
	cout << s.getvalue() << endl;

	Solution& s1 = Solution::getgouzao();
	cout << s1.getvalue() << endl;
	s1.setvalue(100);
	cout << s1.getvalue() << endl;

	
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
