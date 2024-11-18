#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <stack>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:移除一串数字中的K个数字后，得到最小的数字组合
 */

class func
{
public:
	func();
	~func();
	string realize(string num ,int k)
	{
		vector<int> s;
		string result = "";

		/*遍历字符串*/
		for (int i = 0; i < num.length(); i++)
		{
			/*得到整形数字*/
			int number = num[i] - '0';
			
			/*当栈不为空，栈顶元素大于number,可以删除时*/
			while (s.size() != 0 && s[s.size()-1] > number && k > 0)
			{
				s.pop_back();
				k--;
			}

			if (number != 0 || s.size() != 0)
			{
				s.push_back(number);
			}
		}

		/*k还有残留且栈不为空，可以继续删*/
		while (s.size() != 0 && k > 0)
		{
			s.pop_back();
			k--;
		}

		/*遍历栈中元素，存储在result*/
		for (int i = 0; i < s.size(); i++)
		{
			result.append(1, '0' + s[i]); //字符拼接
		}
		if (result == "")
		{
			return "0";
		}
		return result;
	}

private:

};

func::func()
{
	cout << "构造成功！" << endl;
}

func::~func()
{
	cout << "析构成功！" << endl;
}

int main()
{
	func f;
	string num = "1432219";
	cout<<f.realize(num,2)<<endl;
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
