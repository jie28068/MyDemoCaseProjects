#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <map>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:已知字符串pattern与字符串str，确认str是否与pattern匹配。
   *str与pattern匹配代表字符串str中的单词与pattern中的字符一一对应。
   *(其中pattern中只包含小写字符，str中的单词只包含小写字符，使用空格分隔。)
   * Explain:eg:
	*pattern =“abba"', str =“dog cat cat dog”匹配.
	*pattern =“abba", str ="dog cat cat fish”不匹配.
	*pattern = "aaaa" , str = "dog cat cat dog"不匹配.
	*pattern = "abba", str = "dog dog dog dog"不匹配.
 */

class solution
{
public:
	bool func(string str, string pattern)
	{
		/*单词到pattern的映射*/
		map<string, char> hash_map;

		/*已被映射的字符*/
		char user[128] = { 0 };

		/*临时保存的拆分单词*/
		string word;

		/*指向的pattern*/
		int pos = 0;

		/*str尾部push一个空格*/
		str.push_back(' ');

		for (int i = 0; i < str.length(); i++)
		{
			/*遇到空格就拆分出一个新单词*/
			if (str[i] == ' ')
			{
				/*如果分割出来的单词，但pattern没有对应*/
				if (pos == pattern.length())
				{
					return false;
				}

				/*若单词未出现在哈希映射中*/
				if (hash_map.find(word) == hash_map.end())
				{
					/*如果当前pattern字符已使用*/
					if (user[pattern[pos]])
					{
						return false;
					}
					/*键值与实值相对应*/
					hash_map[word] = pattern[pos];
					/*字符数组标记*/
					user[pattern[pos]] = 1;
				}

				else
				{
					/*当前Word已建立映射，无法与当前pattren对应*/
					if(hash_map[word] != pattern[pos])
					{
						return false;
					}
				}

				/*完成一个单词的插入和查询后，清空word*/
				word = "";
				/*指向pattern的指针前移*/
				pos++;
			}
			/*分割单词*/
			else
			{
				word += str[i];
			}
		}
		if (pos != pattern.length())
		{
			/*有多余的pattern字符*/
			return false;
		}
		return true;
	}
};
int main()
{
	string pattern = "abba";
	string str = "dog cat cat dog";
	solution s;
	if (s.func(str, pattern))
	{
		printf("是词语模式\n");
	}
	else
	{
		printf("不是词语模式\n");
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
