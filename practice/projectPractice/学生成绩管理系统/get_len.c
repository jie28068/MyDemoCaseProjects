#include <stdio.h>
#include <stdlib.h>
int get_len(FILE* file_p)//得到文件中字符串的行数
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
	fseek(file_p, 0, SEEK_SET);//回到文件开头位置
	return len;
}