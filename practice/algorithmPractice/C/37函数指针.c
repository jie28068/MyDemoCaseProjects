#include <stdio.h>
/*���Ӻ�����������*/
typedef int (*p)(int a, int b);//����Ƥ

int max(int a, int b)     //��������ʵҲ����һ����ַ
{
	return a > b ? a : b;
}

int main37()
{
	p q = max;      //p����һ������������
	int j = q(1, 2);
	printf("%d\n", j);
	system("pause");
	return 0;
}