#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
/*
	����һ�������ַ�,��������Ԫ����Ϊ��������ͳ��ÿ�������ַ��ĸ���
*/
#include <stdio.h>
int main45()
{
	int i, n, str[10] = { 0 }; char ch;
	printf("������һ�������ַ���");                
	while ((ch = getchar()) != '\n') //������һ��Ҫscanf����fgets�õ����룬getcharҲ����
	{
		if (ch == '0')
			str[9]++;     //ʹ�����������ֳ��ִ����ĸ���
		else if (ch > '0' && ch <= '9')
			str[ch - 49]++;   //ע�����mcs�ַ���0��ʮ������48����������ּ�һ
	}
	for (i = 0; i < 9; i++)
		printf("%d:%d��\n", i + 1, str[i]);
	printf("0:%d��", str[9]);
	system("pause");
	return 0;
}