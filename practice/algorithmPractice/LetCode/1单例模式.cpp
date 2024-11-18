#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:惰性初始化-懒汉式
*/

class Solution
{
public:
	/*删除拷贝和赋值运算符*/
	Solution(const Solution&) = delete;
	Solution& operator= (const Solution&) = delete;

	/*静态获取构造函数*/
	static Solution& getgouzao()
	{
		/*静态构造,并初始化*/
		static Solution s(0);
		/*返回引用*/
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
	/*私有化构造函数*/
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
