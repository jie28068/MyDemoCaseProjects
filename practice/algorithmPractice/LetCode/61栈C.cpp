#if 0
#include <iostream>
using namespace std;
#include <time.h>
const int Size = 100;
//栈结构体
struct Stack
{
	int arr[Size];
	int top;
};

class Solution
{
private:
	struct Stack* st;
public:
	//初始化栈
	Solution() :st(new struct Stack)
	{
		st->top = -1;
		st->arr[Size - 1] = { 0 };
	}

	//入栈
	void PushStack(int a)
	{
		cout << a << " ";
		st->arr[++st->top] = a;
	}

	//出栈
	void PopStack()
	{
		cout << st->arr[st->top] << "  " ;
		st->top--;
	}
};
int main()
{
	Solution s;
	srand((unsigned int)time(NULL));
	for (int i = 0; i < 10; ++i)
	{
		s.PushStack(rand() % 20);
	}
	cout << endl;
	for (int i = 0; i < 10; ++i)
	{
		s.PopStack();
	}
	system("pause");
	return 0;
}
#endif