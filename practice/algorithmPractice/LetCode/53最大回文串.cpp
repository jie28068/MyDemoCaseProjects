#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:ʹ���ַ���s�е��ַ���������ϣ������µ��ַ����������ɵ��ַ���Ϊ�����ַ���,��Ҫ���������ַ��������ַ�ֻҪͷ�����֣�β����Ҫ��Ӧ���֡�
   * ���䳤��
   * eg:a...a�� ccd.. .dcc�� cc...d...cc
 */

class solution
{
public:
	int longgest(string s)
	{
		//�ַ���ϣ
		int char_map[128] = { 0 };
		//ż����ĳ���
		int max_len = 0;
		//�Ƿ������ĵ�
		int flag = 0;

		/*ͳ���ַ����ִ���*/
		for (int i = 0; i < s.length(); i++)
		{
			char_map[s[i]]++;
		}

		/*ĳ���ַ�Ϊż���Ϳ����ڻ��Ĵ���*/
		for (int i = 0; i < 128; i++)
		{
			if (char_map[i] % 2 == 0)
			{
				max_len += char_map[i];
			}
			else
			{
				/*ĳ���ַ���Ϊ������������һ��*/
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
