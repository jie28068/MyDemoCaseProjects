#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//7
int fun7(char* p, int* n)
{
	if (p == NULL || n == NULL)
	{
		return -1;
	}
	int begin = 0;
	int end = strlen(p) - 1;
	//����߿�ʼ
	//�����ǰ�ַ�Ϊ�գ�����û�н���
	while (p[begin] == ' ' && p[begin] != 0)
	{
		begin++; //λ�ô����ƶ�һλ
	}
	//���������ƶ�
	while (p[end] == ' ' && end > 0)
	{
		end--; //�����ƶ�
	}
	if (end == 0)
	{
		return -2;
	}
	//�ǿ�Ԫ�ظ���
	*n = end - begin + 1;
	return 0;
}

int main7(void)
{
	char* p = "      abcddsgadsgefg      ";
	int ret = 0;
	int n = 0;
	ret = fun7(p, &n);
	if (ret != 0)
	{
		return ret;
	}
	printf("�ǿ��ַ���Ԫ�ظ�����%d\n", n);
	//printf("%d\n", ~1);//-2
	//printf("%d\n", ~(-1));//0
	system("pause");
	return 0;
}