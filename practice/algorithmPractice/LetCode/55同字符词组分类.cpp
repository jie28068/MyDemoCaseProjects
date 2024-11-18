#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:����:["eat", "tea", "tan", "ate", "nat", "bat""]
		����:["ate", "eat","tea"],["nat","tan"]��["bat"] ]
 */
class solution
{
public:
	vector<vector<string>> func(vector<string>& strs)
	{
		vector<vector<string>> result;
		map<string, vector<string>> hash_map;

		for (int i = 0; i < strs.size(); i++)
		{
			string str = strs[i];

			/*������ĸ����*/
			sort(str.begin(), str.end());

			/*�ж��Ƿ��ڱ��ڣ����ھ�ӳ��*/
			if (hash_map.find(str) == hash_map.end())
			{
				/*����һ���յ��ַ�������*/
				vector<string> temp;
				hash_map[str] = temp;
			}
			hash_map[str].push_back(strs[i]);
		}
		
		/*������*/
		map<string, vector<string>>::iterator it;
		for (it = hash_map.begin(); it != hash_map.end(); ++it)
		{
			result.push_back((*it).second);
		}
		return result;
	}
};
int main()
{
	vector<string> strs;
	strs.push_back("eat");
	strs.push_back("tea");
	strs.push_back("tan");
	strs.push_back("ate");
	strs.push_back("nat");
	strs.push_back("bta");

	solution s;
	vector<vector<string>> result = s.func(strs);

	for (int i = 0; i < result.size(); i++)
	{
		for (int j = 0; j < result[i].size(); j++)
		{
			printf("  [%s]", result[i][j].c_str());
		}
		printf("\n");
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
