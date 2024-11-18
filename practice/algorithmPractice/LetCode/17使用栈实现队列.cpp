#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stack>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
typedef struct s_list
{
	//数据域
	int data;
	//指针域
	s_list* next;
	//构造函数
	s_list(int x) :data(x), next(NULL) {}
}l_list;

class my_queue
{
public:
	void push(int x)
	{
		std::stack<int > s_temp;
		
		//压入输入的数据
		s_temp.push(x);

		//将原有栈数据情空到临时栈
		while (!s_data.empty())
		{
			s_temp.push(s_data.top());

			//弹栈
			s_data.pop();
		}

		//将临时栈的数据压会原有栈
		while (!s_temp.empty())
		{
			s_data.push(s_temp.top());

			s_temp.pop();
		}
	}

	void pop()
	{
		s_data.pop();
	}

	int peek()
	{
		return s_data.top();
	}

private:
	std::stack<int > s_data;
};
int main()
{
	my_queue q;
	q.push(1);
	q.push(2);
	q.push(3);
	q.push(4);
	cout << q.peek() << endl;
	q.pop();
	cout << q.peek() << endl;
	system("pause");
	return  EXIT_SUCCESS;
}
#endif;
