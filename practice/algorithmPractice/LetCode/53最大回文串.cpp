#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:使用字符串s中的字符，任意组合，生成新的字符串，若生成的字符串为回文字符串,需要除了中心字符，其余字符只要头部出现，尾部就要对应出现。
   * 求其长度
   * eg:a...a、 ccd.. .dcc、 cc...d...cc
 */

class solution
{
public:
	int longgest(string s)
	{
		//字符哈希
		int char_map[128] = { 0 };
		//偶数最长的长度
		int max_len = 0;
		//是否有中心点
		int flag = 0;

		/*统计字符出现次数*/
		for (int i = 0; i < s.length(); i++)
		{
			char_map[s[i]]++;
		}

		/*某个字符为偶数就可以在回文串中*/
		for (int i = 0; i < 128; i++)
		{
			if (char_map[i] % 2 == 0)
			{
				max_len += char_map[i];
			}
			else
			{
				/*某个字符串为奇数个，则丢弃一个*/
				max_len += char_map[i] - 1;
				flag = 1;
			}
		}
		return max_len + flag;
	}
};
int main()
{
	string s = "abccccddIaalaaa";
	solution ss;
	printf("%d\n", ss.longgest(s));
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
