/*
 * @Author: your name
 * @Date: 2021-10-27 16:23:42
 * @LastEditTime: 2021-10-27 20:09:36
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /桌面/2电子相册/inc/touch.h
 */
#ifndef _TOUCH_H_
#define _TOUCH_H_

/*头文件*/
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>//close的头文件
#include <sys/ioctl.h>//ioctl的头文件
/*linux输入子系统的头文件*/
#include <linux/input.h>
/*宏定义*/
#define TOUCH_PATH "/dev/input/event0"
using namespace std;
//extern void touch_xy();
int get_xy();
#endif
