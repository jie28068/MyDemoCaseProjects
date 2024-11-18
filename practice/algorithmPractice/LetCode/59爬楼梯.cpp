#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */

class Solution
{
public:
	int func(int n)
	{
		vector<int> dp(n+3,0);
		dp[1] = 1;
		dp[2] = 2;
		for (int i = 3; i <= n; i++)
		{
			dp[i] = dp[i - 1] + dp[i - 2];
		}
		return dp[n];
	}
};


int fun(int n)
{

	if (n == 1 || n == 2)
	{
		return n;
	}
	return fun(n - 1) + fun(n - 2);
}
#include <string>
int main()
{
	Solution s;
	cout << s.func(10) << endl;
	cout << fun(10) << endl;
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
