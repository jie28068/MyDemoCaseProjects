#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>


int main(int argc, char* argv[])
{
	int fd = open(argv[1], O_RDWR | O_EXCL | O_CREAT | O_TRUNC, 0744);
	struct stat* s = NULL; //定义系统结构体
	stat(argv[1], s);
	printf("文件大小：%d\n文件类型和权限:%s\n硬链接数:%d\n", s->st_size, s->st_mode, s->st_nlink);
}