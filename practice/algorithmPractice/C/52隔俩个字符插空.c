#include <stdio.h>

/*
	�Ӽ�������һ���ַ������´�����ԭ����,ÿ�����ַ�֮�����һ���ո�,
	��ԭ��Ϊabcd,���´�Ϊab cd��Ҫ���ں���insert������´��Ĳ����r���ں��������������Ӧ������������
*/
int main52()
{
	int len, i, index = 0;
	char buf[100] = { 0 };
	printf("�������ַ���:");
	gets(buf);
	len = strlen(buf);
	char arr[100] = { 0 };
	char* p = arr;
	for (i = 0; i < len; i++)
	{
		if (i != 0 && i % 2 == 0)//ȡ�������ÿ�
		{
			p[index] = ' ';
			index++;
		}
		p[index] = buf[i];
		index++;
	}
	p[index] = '\0';
	printf("�޸ĺ���ַ���:%s\n", p);
	system("pause");
	return 0;
}