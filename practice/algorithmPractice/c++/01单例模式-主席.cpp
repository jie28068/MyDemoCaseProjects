#if 0
#include <iostream>
using namespace std;
//����ģʽ��Ϊ�������ж��󣬲���ֻ֤��һ������ʵ��
//������ϯ��
class chairMan
{
public:
	static chairMan* get()
	{
		return singleMan;
	}
	void ppp(string txt)
	{
		cout << txt << endl;
	}
private:
	static chairMan* singleMan;
//private:
	chairMan()
	{	
		cout << "������ϯ" << endl;
	}
	chairMan(const chairMan& c)
	{
		cout << "**************" << endl;
	}
};
chairMan* chairMan::singleMan = new chairMan();
void test()
{
	chairMan *m1= chairMan::get();
	chairMan* m2 = chairMan::get();
	if (m1 == m2)
	{
		cout << "m1��m2��ͬ" << endl;
	}
	else
	{
		cout << "m1��m2����ͬ" << endl;
	}
	/*
	����˽��
	chairMan* m3 = new chairMan(*m2);
	if (m3 == m2)
	{
		cout << "m3��m2��ͬ" << endl;
	}
	else
	{
		cout << "m3��m2����ͬ" << endl;
	}*/
}

int main()
{
	cout << "main����" << endl;
	test();
	system("pause");
	return EXIT_SUCCESS;
}
#endif;