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
//		printf("输入的参数过少");
//	}
//	FILE* p = fopen(argv[1], "r");
//	FILE* p1 = fopen(argv[2], "w");
//	if (!p || !p1)
//	{
//		return -1;
//	}
//	struct stat* s = NULL; //定义系统结构体
//	stat(argv[1], s);  //
//	char* ch;
//	int maxsize = 0;
//	if (s->st_size < MAXSIZE) //得到要拷贝文件的大小
//	{
//		maxsize = s->st_size;
//		ch = (char*)malloc(sizeof(char) * s->st_size);
//	}
//	else
//	{
//		maxsize = MAXSIZE;
//		ch = (char*)malloc(sizeof(char) * MAXSIZE);
//	}
//	while (!feof(p))   //判断文件是否到结尾
//	{
//		memset(ch, 0, maxsize);
//		int len = fread(ch, 1, maxsize, p);  //读文件
//		fwrite(ch, len, 1, p1);     //写文件
//	}
//	unsigned int end_time = time(NULL);
//
//	fclose(p);
//	fclose(p1);
//	free(ch);
//	printf("耗时：%d(s)", end_time - star_time);
//	return 0;
//}