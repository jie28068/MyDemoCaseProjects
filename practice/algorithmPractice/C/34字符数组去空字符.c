#include <stdio.h>

void To_empty(char* Str)
{
	char* p = Str;
	while (*Str != '\0')
	{
		if (*Str != ' ')
		{
			*p++ = *Str;     //����ǿ�֮��Ͱ�����ֵ���һ��ֵ���н���
		}
		Str++;
	}
	*p = '\0';  //ѭ���������һ��б��0��Ϊ������
}

int main34()
{
	char arr[100];
	printf("������һ���ַ���\n");
	fgets(arr, sizeof(arr), stdin);
	To_empty(arr);
	printf("ȥ�պ���ַ���Ϊ��%s\n",arr);
	system("pause");
	return 0;
}
