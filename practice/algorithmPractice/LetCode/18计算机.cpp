#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <stack>
using namespace std;
/*
   * User:SJ
   * Time:2021/10/17
   * Function:计算机的加减，括号的实现
   * Explain:
   *思想一：
      利用有限自动状态机，数字栈符号栈。碰到运算符时标志位置一，为左括号置零不能计算。
      字符和数字转状态时要指针回退
   *思想二：
      秒！鬼才想出的！
 */
//类
class List
{
public:
	/*思想一*/
	 int calc(string s);
	 void compure(stack<int>&, stack <char> &);

	 /*思想二：特别简洁，用一个栈维护主一个符号位*/
	 int calculate2(string s) 
	 {
		 stack<int> st; // 存储正负号
		 int ans = 0, num = 0, op = 1;//此次整数值，计数器，符号位
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
				 ans += op * num;    //得到此次整数值
				 num = 0;           //重置计数器

				 if (s[i] == '+')
				 {
					 op = st.top();  //为加号标记位为1
				 }
				 else if (s[i] == '-')
				 {
					 op = -st.top();  //为减号标记位为-1
				 }
				 else if (s[i] == '(')
				 {					
					 st.push(op);      // 将括号前符号放入栈顶
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

//计算方式的实现
void List::compure(stack<int>& number_stack, stack<char>& operation_stack)
{
	if (number_stack.size() < 2)
	{
		return;
	}

	/*分别拿到数字栈的第一和第二号元素*/
	int num1 = number_stack.top();
	number_stack.pop();
	int num2 = number_stack.top();
	number_stack.pop();

	/*判断字符栈顶的符号*/
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
	/*弹栈*/
	operation_stack.pop();
}

//栈
int List::calc(string s)
{
	const int star_begin = 0;  //开始状态
	const int number_state = 1;  //数字状态
	const int operation_state = 2;  //符号状态
	int falg = 0;    //标志位
	int state = star_begin;   //起始位
	stack<int> number_stack;  //数字栈
	stack <char> operation_stack; //符号栈
	double number = 0;  //数字类型转换使用
	int i = 0;  //

	//遍历字符串
	for (i = 0; i < s.length(); i++)
	{
		//为空就跳过这个字符
		if (s[i] == ' ')
		{
			continue;
		}

		//有限自动状态机
		switch (state)     //表达式
		{
			//开始状态
		case star_begin:
			if (s[i] >= '0' && s[i] <= '9')   //进入数字状态
			{
				state = number_state;
			}
			else       //进入符号状态
			{
				state = operation_state;
			}
			/*指针退格*/
			i--;    
			break;

			//数字状态
		case number_state:
			if (s[i] >= '0' && s[i] <= '9')   
			{
				/*将字符串转换为整形数字*/
				number = number * 10 + s[i] - '0';
			}		
			else
			{
				/*如果不是数字就把之前的数字压入数字栈*/
				number_stack.push(number);
				if (falg == 1)
				{
					/*标志位为一就进行计算*/
					compure(number_stack, operation_stack);
				}
				/*数字清零*/
				number = 0;
				/*指针退格*/
				i--;
				/*进入符号状态*/
				state = operation_state;
			}
			break;

			//符号状态
		case operation_state:
			if (s[i] == '+' || s[i] == '-' )
			{
				/*压入符号栈*/
				operation_stack.push(s[i]);
				/*标志位置一*/		
			
					falg = 1;			
			}
			else if (s[i] == '(')
			{
				/*转入数字状态*/
				state = number_state;
				falg = 0;
			}
			else if (s[i] >= '0' && s[i] <= '9')
			{
				/*转入数字状态*/
				state = number_state;
				/*指针回退*/
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

	/*括号后面有数字继续加上*/
	if (number != 0)
	{
		number_stack.push(number);
		compure(number_stack, operation_stack);
	}

	/*为零的特殊情况*/
	if (number == 0 && number_stack.empty())
	{
		return 0;
	}

	/*返回数字栈栈顶*/
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
