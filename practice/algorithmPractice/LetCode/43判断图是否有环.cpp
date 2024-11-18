#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <queue>
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
struct grapnode
{
	int value;
	const char* data;
	vector<grapnode*> night;
	grapnode(const char* x, int y) :data(x), value(y) {};
};
int main()
{

	system("pause");
	return  EXIT_SUCCESS;
}
#endif
