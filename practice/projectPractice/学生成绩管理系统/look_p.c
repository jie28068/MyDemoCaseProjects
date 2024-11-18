#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "student.h"
/*
   * User:SJ
   * Time:2021/8/17
   * Function:
   * Explain:
 */
void look_p()
{
	FILE* lp = fopen("../text.txt", "r+");
	if (NULL == lp)
		return;
	int len = get_len(lp);
	int i = 0;
	int a = 0;
	char buf[1024] = { 0 };
	printf("学号 姓名 成绩 性别(男XY,女X)\n");
	while (fgets(buf, 1024, lp))
	{	
		//memset(buf, 0, 1024);
		printf("%s\n", buf);
	}
	fclose(lp);
}
