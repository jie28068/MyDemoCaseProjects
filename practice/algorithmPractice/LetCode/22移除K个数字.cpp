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
   * Explain:�Ƴ�һ�������е�K�����ֺ󣬵õ���С���������
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

		/*�����ַ���*/
		for (int i = 0; i < num.length(); i++)
		{
			/*�õ���������*/
			int number = num[i] - '0';
			
			/*��ջ��Ϊ�գ�ջ��Ԫ�ش���number,����ɾ��ʱ*/
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

		/*k���в�����ջ��Ϊ�գ����Լ���ɾ*/
		while (s.size() != 0 && k > 0)
		{
			s.pop_back();
			k--;
		}

		/*����ջ��Ԫ�أ��洢��result*/
		for (int i = 0; i < s.size(); i++)
		{
			result.append(1, '0' + s[i]); //�ַ�ƴ��
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
	cout << "����ɹ���" << endl;
}

func::~func()
{
	cout << "�����ɹ���" << endl;
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
