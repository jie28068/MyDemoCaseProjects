#include <stdio.h>

/*�������ַ���ֵ����һ�ַ�������*/
char* My_StrNcpy(char* dest, char* src, int n)
{
	for (int i = 0; i < n; i++)
	{
		dest[i] = src[i];
	}
	dest[n] = '\0';
	return dest;
}

/*�ݹ���������ַ����ĳ���*/
int my_strlen(char* str)
{
	if (*str == '\0')   
		return 0;	
	else
		return 1 + my_strlen(str + 1);
}

int main33()
{
	char dest[100];
	char src[100] = { 0 };
	int n = 0;
	printf("�������ַ��ܣ�\n");
	fgets(src, sizeof(src), stdin);
	printf("������Ҫ���Ƶĳ��ȣ�\n");
	scanf_s("%d", &n);
	int len = my_strlen(src);
	if (n <= len && n >= 0)
	{
		char* p = My_StrNcpy(dest, src, n);
		printf("���ƺ���ַ�Ϊ��%s\n", p);
	}
	else
	{
		printf("��������ȷ�����֣�");
	}

	system("pause");
	return 0;
}