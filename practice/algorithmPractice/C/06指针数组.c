#include <stdio.h>

//argc: �������ĸ�����������ִ�г���
//argv��ָ�����飬ָ������Ĳ���
int main6(int argc, char* argv[])
{

	//ָ�����飬�������飬ÿ��Ԫ�ض���ָ��
	char* a[] = { "aaaaaaa", "bbbbbbbbbb", "ccccccc" };
	printf("%p\n", &a[0]);//������׵�ַ�����������ַ��ͬ
	printf("%p\n", a);
	printf("%p\n", a[0]);//����ֵ��aaaaaaa����ַ
	int b[] = { 1,2,3,4};
	printf("\n");
	printf("%p\n", b);
	printf("%d\n", b[0]);
	printf("%p\n", &b[0]);
	
	int i = 0;
	printf("argc = %d\n", argc);
	for (i = 0; i < argc; i++)
	{
		printf("%s\n", argv[i]);
	}
	system("pause");
	return 0;
}