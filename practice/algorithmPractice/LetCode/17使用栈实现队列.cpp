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
	//������
	int data;
	//ָ����
	s_list* next;
	//���캯��
	s_list(int x) :data(x), next(NULL) {}
}l_list;

class my_queue
{
public:
	void push(int x)
	{
		std::stack<int > s_temp;
		
		//ѹ�����������
		s_temp.push(x);

		//��ԭ��ջ������յ���ʱջ
		while (!s_data.empty())
		{
			s_temp.push(s_data.top());

			//��ջ
			s_data.pop();
		}

		//����ʱջ������ѹ��ԭ��ջ
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
