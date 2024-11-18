#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <stack>
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */

int main()
{
	stack<int> s;
	if (s.empty())
	{
		cout << "This stack is stack" << endl;
	}
	s.push(0);
	s.push(5);
	s.push(2);
	cout << s.top() << endl;
	s.pop();
	//s.pop();

	cout << s.top() << endl;
	cout << s.size() << endl;

	system("pause");
	return  EXIT_SUCCESS;
}
#endif;
