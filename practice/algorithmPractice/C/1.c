#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>


int main(int argc, char* argv[])
{
	int fd = open(argv[1], O_RDWR | O_EXCL | O_CREAT | O_TRUNC, 0744);
	struct stat* s = NULL; //����ϵͳ�ṹ��
	stat(argv[1], s);
	printf("�ļ���С��%d\n�ļ����ͺ�Ȩ��:%s\nӲ������:%d\n", s->st_size, s->st_mode, s->st_nlink);
}