//#include<stdio.h>
//#include<time.h>	//时间函数 
//#include<stdlib.h>	//随机函数需要的库 
//#include <conio.h>  //char ch = _getch(); //需要头文件#include <conio.h>*/
////常量  SIZE不可变 
//#define SIZE 4
////空格数量
//int num = 16;
////实际的数据
//int a[SIZE][4] = { 0 };
////随机区间 
//int b[10] = { 2,2,2,2,2,2,4,2,2,2 };
//int x1 = 0, y1 = 0, num1 = 0; //全局变量，用于新增表格的赋值
//void create(int l)
//{
//	int i, j, count;
//	int k, n, m;
//	//设置随机数
//	srand(time(NULL));
//	for (m = 0; m < l; m++)
//	{
//		count = 0;
//		n = rand() % num + 1;//产生随机数的位置
//		k = rand() % 10;//取出数组b[]中的一个数字
//		for (i = 0; i < SIZE; i++)
//		{
//			for (j = 0; j < SIZE; j++)
//			{
//				if (a[i][j] == 0)//为零时计数位才会加加，所以必不可能和上一次位置发生冲突
//				{
//					count++;
//				}
//				if (count == n)//产生随机数的位置
//				{
//					a[i][j] = b[k];
//					num--;      
//					goto loop;  //跳出本次循环
//				}
//			}
//		}
//	loop:
//		{}
//	}
//}
//void interface_2048(void)
//{
//	//有警告就改成  const char *s=" +——-+"; 
//	char* s = " +——-+";
//	int i, j, k = 0, n;
//	for (i = 0; i < SIZE; i++)
//	{
//		for (j = 0; j < SIZE; j++)
//		{
//			printf("%s", s);
//		}
//		putchar('\n');
//		for (j = 0; j < 4; j++)
//		{
//			if (a[i][j] == 0)
//			{
//				printf(" |     |");
//				continue;               //跳过本次循环
//			}
//			n = 1;
//			printf(" |");
//			k = a[i][j];
//			do
//			{ 
//				n++;                  //求出加了几次
//			} while ((k /= 10) > 9); 
//			if (n < 3)                            //为了使数字居中
//			{
//				printf("  %-3d|", a[i][j]);
//			}
//			else if (n < 5)
//			{
//				printf(" %-4d|", a[i][j]);
//			}
//			else
//			{
//				printf("%5d|", a[i][j]);
//			}
//		}
//		putchar('\n');
//		for (j = 0; j < SIZE; j++)
//		{
//			printf("%s", s);
//		}
//		putchar('\n');
//	}
//	printf("请选择移动方向上、下、左、右（w、s、a、d）\n——>");
//}
//void left()
//{
//	/*左移*/
//	int i, j, z, tmp;
//	for (i = 0; i < 4; i++)//如果这个数字为零
//		for (j = 0; j < 4; j++)
//			if (a[i][j] == 0)   
//				for (z = j + 1; z < 4; z++)  //就找这个数字的右边一个数字
//					if (a[i][z] > 0)   //如果这个数字的右边的数字不为零，就将这个数字和左边的零进行交换
//					{
//						tmp = a[i][j];  //现实左移的效果
//						a[i][j] = a[i][z];
//						a[i][z] = tmp;
//						break;
//					}
//	for (i = 0; i < 4; i++)//如果方格数字相同则
//		for (j = 0; j < 4; j++)
//			if (a[i][j] > 0)      
//				for (z = j + 1; z < 4; z++)
//					if (a[i][z] > 0)       //通过三个if语句判断
//						if (a[i][z] == a[i][j])  //如果左右俩边的数字相同
//						{
//							a[i][j] *= 2;    //就左边*2，右边置零
//							a[i][z] = 0;
//						}
//						else
//							break;
//					else
//						break;
//			else
//				break;
//}
//void right()
//{
//	int i, j, z, tmp;
//	for (i = 0; i < 4; i++)
//		for (j = 3; j >= 0; j--)
//			if (a[i][j] == 0)
//				for (z = j - 1; z >= 0; z--)
//					if (a[i][z] > 0)
//					{
//						tmp = a[i][j];
//						a[i][j] = a[i][z];
//						a[i][z] = tmp;
//						break;
//					}
//	for (i = 0; i < 4; i++)
//		for (j = 3; j >= 0; j--)
//			if (a[i][j] > 0)
//				for (z = j - 1; z >= 0; z--)
//					if (a[i][z] > 0)
//						if (a[i][z] == a[i][j])
//						{
//							a[i][j] *= 2;
//							a[i][z] = 0;
//						}
//						else
//							break;
//					else
//						break;
//			else
//				break;
//}
//void up()
//{
//	int i, j, z, tmp;
//	for (i = 0; i < 4; i++)
//		for (j = 0; j < 4; j++)
//			if (a[j][i] == 0)
//				for (z = j + 1; z < 4; z++)
//					if (a[z][i] > 0)
//					{
//						tmp = a[j][i];
//						a[j][i] = a[z][i];
//						a[z][i] = tmp;
//						break;
//					}
//	for (i = 0; i < 4; i++)
//		for (j = 0; j < 4; j++)
//			if (a[j][i] > 0)
//				for (z = j + 1; z < 4; z++)
//					if (a[z][i] > 0)
//						if (a[z][i] == a[j][i])
//						{
//							a[j][i] *= 2;
//							a[z][i] = 0;
//						}
//						else
//							break;
//					else
//						break;
//			else
//				break;
//}
//void down()
//{
//	int i, j, z, tmp;
//	for (i = 0; i < 4; i++)
//		for (j = 3; j >= 0; j--)
//			if (a[j][i] == 0)   //行列交换下标位置
//				for (z = j - 1; z >= 0; z--)
//					if (a[z][i] > 0)
//					{
//						tmp = a[j][i];
//						a[j][i] = a[z][i];
//						a[z][i] = tmp;
//						break;
//					}
//	for (i = 0; i < 4; i++)
//		for (j = 3; j >= 0; j--)
//			if (a[j][i] > 0)
//				for (z = j - 1; z >= 0; z--)
//					if (a[z][i] > 0)
//						if (a[z][i] == a[j][i])
//						{
//							a[j][i] *= 2;
//							a[z][i] = 0;
//						}
//						else
//							break;
//					else
//						break;
//			else
//				break;
//}
//void putt(char ch)
//{
//	switch (ch)
//	{
//	case 'a':
//		left();
//		break;
//	case 'd':
//		right();
//		break;
//	case 'w':
//		up();
//		break;
//	case 's':
//		down();
//		break;
//	}
//}
//int new()//新增格字
//{
//	srand((unsigned int) time(NULL));
//    int x = 0, y = 0, num = 0;
//	int fals = 1;
//    do {     
//        int i, j;
//        for (i = 0; i < 4; i++)
//            for (j = 0; j < 4; j++)
//                if (a[i][j] == 0)   //只要有位置标志位就是零
//                    fals = 0;
//        if (fals) //新格无法被填入，游戏失败 
//        {
//            system("cls");
//			return 1;
//        }
//        x = rand() % 4;
//        y = rand() % 4;
//        num = rand() % 2;  
//		int k = rand() % 10;
//        if (num == 0)
//            num = b[k];
//        else if (num == 1)
//            num = b[k];
//    } while (a[x][y] > 0); //非0才会被赋新值
//    x1 = x;
//    y1 = y;
//    num1 = num;
//    a[x1][y1] = num1;
//}
//int main44()
//{
//	printf("输入任意字符开始,按ESC键退出\n");
//	create(2);	
//	interface_2048();
//	while (1)
//	{
//		char ch = _getch();
//		if (ch == 'a' || ch == 'd' || ch == 's' || ch == 'w')
//		{
//			system("cls");
//			putt(ch);
//			interface_2048();
//			int i=new();
//			if (1 == i)
//				break;
//		}
//		if (ch == 27)
//		{
//			system("cls");
//			break;
//		}
//	}
//	printf("game over!!!!!!!!\n");
//	system("pause");
//	return 0;
//}
