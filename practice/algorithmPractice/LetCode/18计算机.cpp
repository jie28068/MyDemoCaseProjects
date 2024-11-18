#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <stack>
using namespace std;
/*
   * User:SJ
   * Time:2021/10/17
   * Function:������ļӼ������ŵ�ʵ��
   * Explain:
   *˼��һ��
      ���������Զ�״̬��������ջ����ջ�����������ʱ��־λ��һ��Ϊ���������㲻�ܼ��㡣
      �ַ�������ת״̬ʱҪָ�����
   *˼�����
      �룡�������ģ�
 */
//��
class List
{
public:
	/*˼��һ*/
	 int calc(string s);
	 void compure(stack<int>&, stack <char> &);

	 /*˼������ر��࣬��һ��ջά����һ������λ*/
	 int calculate2(string s) 
	 {
		 stack<int> st; // �洢������
		 int ans = 0, num = 0, op = 1;//�˴�����ֵ��������������λ
		 st.push(op);

		 for (int i = 0; i < s.length(); i++)
		 {
			 if (s[i] == ' ')
			 {
				 continue;
			 }
			 else if (s[i] >= '0' && s[i] <= '9')
			 {
				 num = num * 10 - '0' + s[i];
			 }
			 else 
			 {
				 ans += op * num;    //�õ��˴�����ֵ
				 num = 0;           //���ü�����

				 if (s[i] == '+')
				 {
					 op = st.top();  //Ϊ�Ӻű��λΪ1
				 }
				 else if (s[i] == '-')
				 {
					 op = -st.top();  //Ϊ���ű��λΪ-1
				 }
				 else if (s[i] == '(')
				 {					
					 st.push(op);      // ������ǰ���ŷ���ջ��
				 } 
				 else
				 {
					 st.pop();
				 }
			 }
		 }
		 return ans + op * num;
	 }
};

//���㷽ʽ��ʵ��
void List::compure(stack<int>& number_stack, stack<char>& operation_stack)
{
	if (number_stack.size() < 2)
	{
		return;
	}

	/*�ֱ��õ�����ջ�ĵ�һ�͵ڶ���Ԫ��*/
	int num1 = number_stack.top();
	number_stack.pop();
	int num2 = number_stack.top();
	number_stack.pop();

	/*�ж��ַ�ջ���ķ���*/
	if (operation_stack.top() == '+')
	{
		number_stack.push(num1 + num2);
	}
	else if (operation_stack.top() == '-')
	{
		number_stack.push(num2 - num1);
	}
	/*else if (operation_stack.top() == '*')
	{
		number_stack.push(num2 * num1);
	}
	else if (operation_stack.top() == '/')
	{
		number_stack.push(num2 / num1);
	}*/
	/*��ջ*/
	operation_stack.pop();
}

//ջ
int List::calc(string s)
{
	const int star_begin = 0;  //��ʼ״̬
	const int number_state = 1;  //����״̬
	const int operation_state = 2;  //����״̬
	int falg = 0;    //��־λ
	int state = star_begin;   //��ʼλ
	stack<int> number_stack;  //����ջ
	stack <char> operation_stack; //����ջ
	double number = 0;  //��������ת��ʹ��
	int i = 0;  //

	//�����ַ���
	for (i = 0; i < s.length(); i++)
	{
		//Ϊ�վ���������ַ�
		if (s[i] == ' ')
		{
			continue;
		}

		//�����Զ�״̬��
		switch (state)     //���ʽ
		{
			//��ʼ״̬
		case star_begin:
			if (s[i] >= '0' && s[i] <= '9')   //��������״̬
			{
				state = number_state;
			}
			else       //�������״̬
			{
				state = operation_state;
			}
			/*ָ���˸�*/
			i--;    
			break;

			//����״̬
		case number_state:
			if (s[i] >= '0' && s[i] <= '9')   
			{
				/*���ַ���ת��Ϊ��������*/
				number = number * 10 + s[i] - '0';
			}		
			else
			{
				/*����������־Ͱ�֮ǰ������ѹ������ջ*/
				number_stack.push(number);
				if (falg == 1)
				{
					/*��־λΪһ�ͽ��м���*/
					compure(number_stack, operation_stack);
				}
				/*��������*/
				number = 0;
				/*ָ���˸�*/
				i--;
				/*�������״̬*/
				state = operation_state;
			}
			break;

			//����״̬
		case operation_state:
			if (s[i] == '+' || s[i] == '-' )
			{
				/*ѹ�����ջ*/
				operation_stack.push(s[i]);
				/*��־λ��һ*/		
			
					falg = 1;			
			}
			else if (s[i] == '(')
			{
				/*ת������״̬*/
				state = number_state;
				falg = 0;
			}
			else if (s[i] >= '0' && s[i] <= '9')
			{
				/*ת������״̬*/
				state = number_state;
				/*ָ�����*/
				i--;
			}
			else if (s[i] == ')')
			{
				compure(number_stack, operation_stack);
			}

			break;

		default:
			break;
		}
	}

	/*���ź��������ּ�������*/
	if (number != 0)
	{
		number_stack.push(number);
		compure(number_stack, operation_stack);
	}

	/*Ϊ����������*/
	if (number == 0 && number_stack.empty())
	{
		return 0;
	}

	/*��������ջջ��*/
	return number_stack.top();
}

int main()
{
	string s = "1+2-(3+4)";
	List l;
	cout << l.calc(s) << endl;
	cout << l.calculate2(s) << endl;
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
