/*
 * @Author: your name
 * @Date: 2021-10-27 16:24:37
 * @LastEditTime: 2021-10-28 12:27:19
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /桌面/2电子相册/src/touch.cpp
 */
#include "touch.h"

int get_xy()
{
	//打开文件
	int fd = open("/dev/input/event0", O_RDONLY);
	if (fd == -1)
	{
		perror("open ev err");
		return -1;
	}

	//初始化结构体
	struct input_event ev;

	//初始化俩组int，用来保存按与松的坐标
	int x0 = -1, y0 = -1, x = -1, y = -1;

	//循环
	while (1)
	{
		//读取文件
		int r = read(fd, &ev, sizeof(ev));
		if (r != sizeof(ev))
		{
			continue;
		}

		//事件是触摸屏事件，压力值
		if (ev.type == EV_ABS)
		{
			//x轴
			if (ev.code == ABS_X)
			{
				//若按下前，未被附过值
				if (x0 == -1)
				{
					//按坐标
					x0 = ev.value;
				}
				//松坐标
				x = ev.value;
			}
			//y轴
			if (ev.code == ABS_Y)
			{
				//若按下前，未被附过值
				if (y0 == -1)
				{
					//按坐标
					y0 = ev.value;
				}
				//松坐标
				y = ev.value;
			}
		}

		//事件为按键事件
		if (ev.type == EV_KEY && ev.code == BTN_TOUCH)
		{
			//值为零，即松开的时刻
			if (ev.value == 0)
			{
				//结束循环
				break;
			}
		}

	}

	cout << "x:" << x << " " << "x0:" << x0 << " " << "y:" << y << " " << "y0:" << y0 << endl;

	/*通过俩组坐标差值得到手势*/
	int xv = (x0 - x > 0) ? (x0 - x) : (-(x0 - x));
	int yv = (y0 - y > 0) ? (y0 - y) : (-(y0 - y));

	if (xv > yv)
	{
		if (x0 - x > 0)
		{
			return 1;
		}
		return 2;
	}
	else if (xv == yv)
	{
		return 5;
	}
	else
	{
		if (y0 - y > 0)
		{
			return 3;
		}
		return 4;
	}

}
