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
class score;  //��Ҫ����
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
	friend void student::show_student(score& sc);//��Ԫ��������
private:
	int min;
	int fin;
};
//����
void student::show_student(score& sc)
{
	cout << "������" << name << endl << "ѧ�ţ�" << number << endl;
	cout << "���гɼ���" << sc.min << endl << "��ĩ��Ա��" << sc.fin << endl;
}
int main()
{
	score sc1(90, 92);
	student stu1("����", 12);
	stu1.show_student(sc1);
	system("pause");
	return  EXIT_SUCCESS;
}
#endif;
