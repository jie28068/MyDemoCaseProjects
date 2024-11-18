#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <map>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:��֪�ַ���pattern���ַ���str��ȷ��str�Ƿ���patternƥ�䡣
   *str��patternƥ������ַ���str�еĵ�����pattern�е��ַ�һһ��Ӧ��
   *(����pattern��ֻ����Сд�ַ���str�еĵ���ֻ����Сд�ַ���ʹ�ÿո�ָ���)
   * Explain:eg:
	*pattern =��abba"', str =��dog cat cat dog��ƥ��.
	*pattern =��abba", str ="dog cat cat fish����ƥ��.
	*pattern = "aaaa" , str = "dog cat cat dog"��ƥ��.
	*pattern = "abba", str = "dog dog dog dog"��ƥ��.
 */

class solution
{
public:
	bool func(string str, string pattern)
	{
		/*���ʵ�pattern��ӳ��*/
		map<string, char> hash_map;

		/*�ѱ�ӳ����ַ�*/
		char user[128] = { 0 };

		/*��ʱ����Ĳ�ֵ���*/
		string word;

		/*ָ���pattern*/
		int pos = 0;

		/*strβ��pushһ���ո�*/
		str.push_back(' ');

		for (int i = 0; i < str.length(); i++)
		{
			/*�����ո�Ͳ�ֳ�һ���µ���*/
			if (str[i] == ' ')
			{
				/*����ָ�����ĵ��ʣ���patternû�ж�Ӧ*/
				if (pos == pattern.length())
				{
					return false;
				}

				/*������δ�����ڹ�ϣӳ����*/
				if (hash_map.find(word) == hash_map.end())
				{
					/*�����ǰpattern�ַ���ʹ��*/
					if (user[pattern[pos]])
					{
						return false;
					}
					/*��ֵ��ʵֵ���Ӧ*/
					hash_map[word] = pattern[pos];
					/*�ַ�������*/
					user[pattern[pos]] = 1;
				}

				else
				{
					/*��ǰWord�ѽ���ӳ�䣬�޷��뵱ǰpattren��Ӧ*/
					if(hash_map[word] != pattern[pos])
					{
						return false;
					}
				}

				/*���һ�����ʵĲ���Ͳ�ѯ�����word*/
				word = "";
				/*ָ��pattern��ָ��ǰ��*/
				pos++;
			}
			/*�ָ��*/
			else
			{
				word += str[i];
			}
		}
		if (pos != pattern.length())
		{
			/*�ж����pattern�ַ�*/
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
		printf("�Ǵ���ģʽ\n");
	}
	else
	{
		printf("���Ǵ���ģʽ\n");
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
