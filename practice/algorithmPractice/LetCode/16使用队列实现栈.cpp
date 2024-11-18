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
	//������
	int data;
	//ָ����
	s_list* next;
	//���캯��
	s_list(int x) :data(x), next(NULL) {}
}l_list;

class mystack
{
public:
	void push(int x)
	{
		//��ʱ����q
		queue<int > q;
		q.push(x);

		while (!qa.empty())
		{
			//������ʱ����
			q.push(qa.front());
			//����
			qa.pop();
		}
		while (!q.empty())
		{	
			//����ԭ����
			qa.push(q.front());
			//����
			q.pop();
		}
	}

	//�Զ�����ӽӿ�
	int pop()
	{
		int x = qa.front();
		qa.front();
		return x;
	}

	//�Զ���Ķ�ͷ�ӿ�
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
