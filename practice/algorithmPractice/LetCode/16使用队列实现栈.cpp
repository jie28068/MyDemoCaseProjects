#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <queue>
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

class mystack
{
public:
	void push(int x)
	{
		//临时队列q
		queue<int > q;
		q.push(x);

		while (!qa.empty())
		{
			//导入临时队列
			q.push(qa.front());
			//出队
			qa.pop();
		}
		while (!q.empty())
		{	
			//导入原队列
			qa.push(q.front());
			//出队
			q.pop();
		}
	}

	//自定义出队接口
	int pop()
	{
		int x = qa.front();
		qa.front();
		return x;
	}

	//自定义的对头接口
	int top()
	{
		return qa.front();
	}

	bool empty()
	{
		return qa.empty();
	}
private:
	queue<int> qa;
};

int main()
{
	mystack m;
	m.push(1);
	m.push(2);
	m.push(3);
	m.push(4);
	cout << m.top();
	m.pop();
	cout << m.top();
	m.push(5);
	cout << m.top();

	system("pause");
	return  EXIT_SUCCESS;
}
#endif;
