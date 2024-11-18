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
        /*Ĭ�Ϲ���*/
        string()
        {
            /*��ʼ��һ�����ַ���*/
            _data = new char[1];
            _data[0] = '\0';
        }

        /*��ȡ���ַ����ĳ���*/
        int size() const
        {
            return strlen(_data);
        }

        /*��������*/
        string(const string& str)
        {
            /*Ҫ���*/
            _data = new char[str.size() + 1];
            cout << "����" << endl;
            strcpy(_data, str._data);
        }

        /*����һ�������Ĺ���*/
        string(const char* str)
        {
            /*���*/
            _data = new char[strlen(str) + 1];
            strcpy(_data, str);
        }

        /*�����*/
        friend ostream& operator<<(ostream& out, string& str);

        /*��ֵ�����*/
        string& operator=(const string& str)
        {

            /*��ֹ���Ҹ�ֵ*/
            if (this == &str)
            {
                return *this;
            }
            cout << "��ֵ" << endl;
            /*ɾ��ԭ�еĿռ�*/
            delete[]_data;

            /*����һ���µĿռ�*/
            _data = new char[str.size() + 1];
            strcpy(_data, str._data);
            return *this;
        }

        /*����*/
        ~string()
        {
            delete[]_data;
        }

        /*���������*/
        char& operator[](int a)
        {
            return _data[a];
        }

        /*�Ӻ������*/
        friend  string operator+(string& str1, string& str2)
        {
            /*����һ���м��ַ���*/
            string sp;  //��Ȼ��new�˿ռ䣬��������_data�ģ�spʵ���ϻ����ڶ��ϣ�����Ҫ����ֵ��������ÿ������캯��
            /*���ٿռ�*/
            sp._data = new char[str1.size() + str2.size() + 1];
            /*�ȸ���*/
            strcpy(sp._data, str1._data);
            /*�����*/
            strcat(sp._data, str2._data);
            /*����ֵ*/
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

