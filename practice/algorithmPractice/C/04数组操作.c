#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main4()
{
	int arry[5];
	printf("%p\n", arry);
	printf("%p\n", &arry[0]);
	printf("%p\n", &arry[4]);
	char a[] = { 'a','b','c' };
	printf("a=%s\n", a);//û���ԡ�\0����β�����Ի�����
	char b[] = { 'a','b','c','\0' };
	printf("b=%s\n", b);
	char buf7[50] = { '1', 'a', 'b', '\0', '7' };
	printf("buf7 = %s\n", buf7);
	//'\ddd'�˽��������ַ���'\xdd'ʮ������ת���ַ�
	// \012�൱��\n
	char str[] = "\012abc";
	printf("str == %s\n", str);

	time_t tm = time(NULL);//�õ�ϵͳʱ��
	srand((unsigned int)tm);//�������ֻ��Ҫ����һ�μ���
	//�ȼ���srand((unsigned int)time(UNLL));
	int r = rand();
	printf("r = %d\n", r);

	system("pause");
	return 0;
}