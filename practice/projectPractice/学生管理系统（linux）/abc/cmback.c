//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
//#include <Windows.h>
#include <stdlib.h>
void cmback()
{
	int a = 0;
	printf("1.回主菜单\t 2.退出\n");
star:
	scanf("%d", &a);
	while (1)
	{
		switch (a)
		{
		case 1:
		//	system("cls");
			muen();
			break;
		case 2:
			exit(0);
			break;
		default:
			printf("请输入正确的指令！(0/1)");
			goto star;
		}
	}
}
