#include <stdio.h>

void main24()
{
    //����ת������������
    int transfer(int x);
    int x;
    printf("������һ��ʮ������:");
    scanf_s("%d", &x);
    printf("ת���ɶ���������:%d\n", transfer(x));
    system("pause");
}
int transfer(int x)
{
    int p = 1, y = 0, yushu;
    while (1)
    {
        yushu = x % 2;
        x /= 2;
        y += yushu * p;
        p *= 10;
        if (x < 2)
        {
            y += x * p;
            break;
        }
    }
    return y;
}