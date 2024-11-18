#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <map>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:s = "abcabcbb"-> "abc",1 
			s = "bbbbb"-> "b",3
			s = "pwwkew" -> "wke",3ע��"pwke"�������ж����Ӵ���
 */
#include <string>
class Solution
{
public:
	int func(string s)
	{
		/*����ͷָ��*/
		int begin = 0;
		int result = 0;
		int char_map[128] = { 0 };
		string word = "";
		for (int i = 0; i < s.length(); i++)
		{
			char_map[s[i]]++;
			/*���û���ʳ��ָ��ַ�*/
			if (char_map[s[i]] == 1)
			{
				word += s[i];
				if (result < word.length())
				{
					result = word.length();
				}
			}

			else
			{
				/*���ظ����ַ�ɾȥ*/
				while (begin < i && char_map[s[i]] > 1)
				{
					char_map[s[begin]]--;
					begin++;
				}
				/*����woord*/
				word = "";
				for (int j = begin; j <= i; j++)
				{
					word += s[j];
				}
			}
		}
		return result;
	}

};
int main()
{
	string s = "pwwkew";
	Solution sr;
	cout << sr.func(s) << endl;
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
