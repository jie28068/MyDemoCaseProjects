#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <map>
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain
 */
#include <vector>

bool is_ok(int map_s[], int map_t[], vector<int>& vec)
{
	for (int i = 0; i < vec.size(); i++)
	{
		if (map_s[vec[i]] < map_t[vec[i]])
		{
			return false;
		}
	}
	return true;
}

int func()
{
	int p = 0x12345678;
	char* fd = (char*)(&p);
	return (fd[0] && 0x78) && (fd[1] && 0x56) && (fd[2] && 0x34) && (fd[3] && 0x12);

}
int main()
{
	//int a = func();
	//if (a)
	//{
	//	printf("小段模式！\n");
	//}
	//else
	//{
	//	printf("大端模式\n");
	//}
	

	system("pause");
	return  EXIT_SUCCESS;
}
#endif
