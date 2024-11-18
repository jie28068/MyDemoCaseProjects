#if 0
#include <iostream>
using namespace std;
//单例模式：为创建类中对象，并保证只有一个对象实例
//创建主席类
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
		cout << "国家主席" << endl;
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
		cout << "m1与m2相同" << endl;
	}
	else
	{
		cout << "m1与m2不相同" << endl;
	}
	/*
	拷贝私有
	chairMan* m3 = new chairMan(*m2);
	if (m3 == m2)
	{
		cout << "m3与m2相同" << endl;
	}
	else
	{
		cout << "m3与m2不相同" << endl;
	}*/
}

int main()
{
	cout << "main调用" << endl;
	test();
	system("pause");
	return EXIT_SUCCESS;
}
#endif;