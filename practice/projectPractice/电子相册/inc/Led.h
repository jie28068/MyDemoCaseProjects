/*
 * @Author: your name
 * @Date: 2021-10-25 17:04:41
 * @LastEditTime: 2021-10-28 15:28:06
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /桌面/2电子相册/inc/Led.h
 */
#ifndef _LED_H
#define _LED_H
#include <iostream>
using namespace std;
#include <stdio.h>    //fopen,
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <list>
#include <time.h>
#include <error.h>
#include <algorithm>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
typedef char* typedata;
#define LCD_PATH "/dev/fb0"
#define GREEN 0xFF00FF
//链表结构体定义
typedef struct lis
{
	/*数据域*/
	typedata data;

	/*后继指针*/
	struct lis* next;
	/*前驱指针*/
	struct lis* per;

	/*构造函数*/
	lis(typedata x):data(x),next(NULL),per(NULL){}
}l_list;

class Led
{
public:
	/*遍历图片目录得到图片*/
	l_list* Traverse();
	void Printf(l_list*p);

	list<string>& T_raverse(list<string>& num);
	void P_rintf(list<string>& num);
	list<string>&  get_T()
	{
		return num;
	}

private:
	list<string> num;
};

int LCD_Init();
void LCD_UNint();
void LCD_Draw_Point(int x,int y,int color);
void LCD_Draw_matrix(int ,int ,int,int ,int);
void BMP(string name, int x0, int y0);
#endif // !LED_H
