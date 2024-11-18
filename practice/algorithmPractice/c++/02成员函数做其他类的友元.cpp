#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
class score;  //先要声明
class student
{
public:
	student(string na,int num)
	{
		this->name = na;
		this->number = num;
	}
	void show_student(score& sc);
private:
	string name;
	int number;
};

class score
{
public:
	score(int m, int n)
	{
		min = m;
		fin = n;
	}
	friend void student::show_student(score& sc);//友元函数声明
private:
	int min;
	int fin;
};
//定义
void student::show_student(score& sc)
{
	cout << "姓名：" << name << endl << "学号：" << number << endl;
	cout << "期中成绩：" << sc.min << endl << "期末成员：" << sc.fin << endl;
}
int main()
{
	score sc1(90, 92);
	student stu1("张三", 12);
	stu1.show_student(sc1);
	system("pause");
	return  EXIT_SUCCESS;
}
#endif;
