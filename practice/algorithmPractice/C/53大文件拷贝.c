//#define _CRT_SECURE_NO_WARNINGS
//#define _CRT_SECURE_NO_WARNINGS
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//#include <string.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//
//#define MAXSIZE 1024*1024*10
//
//int main53(int argc,char * argv[])
//{
//	unsigned int star_time = time(NULL);
//	if (argc < 3)
//	{
//		printf("����Ĳ�������");
//	}
//	FILE* p = fopen(argv[1], "r");
//	FILE* p1 = fopen(argv[2], "w");
//	if (!p || !p1)
//	{
//		return -1;
//	}
//	struct stat* s = NULL; //����ϵͳ�ṹ��
//	stat(argv[1], s);  //
//	char* ch;
//	int maxsize = 0;
//	if (s->st_size < MAXSIZE) //�õ�Ҫ�����ļ��Ĵ�С
//	{
//		maxsize = s->st_size;
//		ch = (char*)malloc(sizeof(char) * s->st_size);
//	}
//	else
//	{
//		maxsize = MAXSIZE;
//		ch = (char*)malloc(sizeof(char) * MAXSIZE);
//	}
//	while (!feof(p))   //�ж��ļ��Ƿ񵽽�β
//	{
//		memset(ch, 0, maxsize);
//		int len = fread(ch, 1, maxsize, p);  //���ļ�
//		fwrite(ch, len, 1, p1);     //д�ļ�
//	}
//	unsigned int end_time = time(NULL);
//
//	fclose(p);
//	fclose(p1);
//	free(ch);
//	printf("��ʱ��%d(s)", end_time - star_time);
//	return 0;
//}