#include <stdio.h>
#include <stdlib.h>
int get_len(FILE* file_p)//�õ��ļ����ַ���������
{
	if (NULL == file_p)
	{
		return;
	}
	char buf[1024] = { 0 };
	int len = 0;
	while (fgets(buf, 1024, file_p))
	{
		++len;
	}
	fseek(file_p, 0, SEEK_SET);//�ص��ļ���ͷλ��
	return len;
}