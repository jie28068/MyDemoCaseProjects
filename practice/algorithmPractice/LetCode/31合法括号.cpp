#if 0
#include <iostream>
using namespace std;
#include <vector>
#include <string>

void gen(string s, int left, int right, vector<string>& result)
{
	/*括号数为零*/
	if (left == 0 && right == 0)
	{
		result.push_back(s);
		return;
	}
	/*左边括号，数量大于0*/
	if (left > 0)
	{
		gen(s + '(', left - 1, right, result);
	}
	/*右边括号数大于左边数*/
	if (left < right)
	{
		gen(s + ')', left, right - 1, result);
	}
}
int main()
{
	vector<string> result;
	gen("", 3, 3, result);
	for (int i = 0; i < result.size(); i++)
	{
		cout << result[i] << endl;
	}
	system("pause");
	return 0;
}

#endif