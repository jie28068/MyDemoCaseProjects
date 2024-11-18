#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <string.h>
using namespace std;

namespace dj
{
    class string
    {
    private:
        char* _data;
    public:
        /*默认构造*/
        string()
        {
            /*初始化一个空字符串*/
            _data = new char[1];
            _data[0] = '\0';
        }

        /*获取该字符串的长度*/
        int size() const
        {
            return strlen(_data);
        }

        /*拷贝构造*/
        string(const string& str)
        {
            /*要深拷贝*/
            _data = new char[str.size() + 1];
            cout << "拷贝" << endl;
            strcpy(_data, str._data);
        }

        /*带有一个参数的构造*/
        string(const char* str)
        {
            /*深拷贝*/
            _data = new char[strlen(str) + 1];
            strcpy(_data, str);
        }

        /*输出流*/
        friend ostream& operator<<(ostream& out, string& str);

        /*赋值运算符*/
        string& operator=(const string& str)
        {

            /*防止自我赋值*/
            if (this == &str)
            {
                return *this;
            }
            cout << "赋值" << endl;
            /*删除原有的空间*/
            delete[]_data;

            /*开辟一个新的空间*/
            _data = new char[str.size() + 1];
            strcpy(_data, str._data);
            return *this;
        }

        /*析构*/
        ~string()
        {
            delete[]_data;
        }

        /*【】运算符*/
        char& operator[](int a)
        {
            return _data[a];
        }

        /*加号运算符*/
        friend  string operator+(string& str1, string& str2)
        {
            /*定义一个中间字符串*/
            string sp;  //虽然我new了空间，但是这是_data的，sp实际上还是在堆上，所以要返回值，他会调用拷贝构造函数
            /*开辟空间*/
            sp._data = new char[str1.size() + str2.size() + 1];
            /*先复制*/
            strcpy(sp._data, str1._data);
            /*后剪切*/
            strcat(sp._data, str2._data);
            /*返回值*/
            return sp;
        }

    };

    ostream& operator<<(ostream& out, string& str)
    {
        out << str._data;
        //cout << "liushifu";
        return out;
    }
}


int main()
{
    dj::string s1("hello");
    //cout << s1 << endl;
    //s1[0] = 'H';
    //cout << s1 << endl;

    dj::string s2("world");

    dj::string s4(s2 + s1);
    cout << s4 << endl;
    system("pause");
    return 0;
}
#endif

