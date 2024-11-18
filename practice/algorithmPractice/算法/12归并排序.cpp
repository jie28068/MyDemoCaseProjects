#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <assert.h>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */

class solution
{
public:
    /*合并俩个有序数组*/
    void cmp(vector<int>& v1, vector<int>& v2, vector<int>& ver)
    {
        /*循环到一个容器结束*/
        int i = 0, j = 0;
        while (i < v1.size() && j < v2.size())
        {
            /*比较*/
            if (v1[i] <= v2[j])
            {
                ver.push_back(v1[i]);
                i++;
            }
            else
            {
                ver.push_back(v2[j]);
                j++;
            }
        }
        /*将多余的插入*/
        for (; i < v1.size(); i++)
        {
            ver.push_back(v1[i]);
        }
        for (; j < v2.size(); j++)
        {
            ver.push_back(v2[j]);
        }
    }

    /*归并*/
    void func(vector<int>& v)
    {
        if (v.size() < 2)
        {
            return;
        }
        int mid = v.size() / 2;
        vector<int> v1;
        vector<int> v2;
        /*大问题拆成小问题*/
        for (int i = 0; i < mid; i++)
        {
            v1.push_back(v[i]);
        }
        for (int i = mid; i < v.size(); i++)
        {
            v2.push_back(v[i]);
        }
        func(v1);
        func(v2);
        v.clear();
        cmp(v1, v2, v);
    }
};
int main()
{
#if 0
    vector<int> v;
    int text[] = { 5,-7,9,8,1,4,-3,10,2,0 };
    for (int i = 0; i < 10; i++)
    {
        v.push_back(text[i]);
    }
    solution s;
    s.func(v);
    for (int i = 0; i < v.size(); i++)
    {
        cout << v[i] << " ";
    }
#else
    vector<int> v1;
    vector<int> v2;
    srand(time(NULL));
    for (int i = 0; i < 10000; i++)
    {
        int num = (rand() * rand()) % 100003;
        v1.push_back(num);
        v2.push_back(num);
    }
    solution s;
    s.func(v1);
    std::sort(v2.begin(), v2.end());
    assert(v1.size() == v2.size());
    cout << "容器相同" << endl;
    assert(1 < 2);
    cout << "数字不同" << endl;
    //for (int i = 0; i < v1.size(); i++)
    //{
    //    assert(v1[i] == v2[i]);
    //}
#endif
    system("pause");
    return EXIT_SUCCESS;
}
#endif
