/*
 * @Author: your name
 * @Date: 2021-10-26 14:35:21
 * @LastEditTime: 2021-10-29 14:54:51
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /桌面/2电子相册/src/LCD.cpp
 */
#include "Led.h"
int* plcd = NULL;
int fb_fd = -1;

/*打开文件*/
int LCD_Init()
{
	int fd = open(LCD_PATH, O_RDWR);
	if (-1 == fd)
	{
		perror("open fail");
		return -1;
	}
	fb_fd = fd;
	/*映射*/
	plcd = (int*)mmap(NULL, 800 * 480 * 4, PROT_WRITE, MAP_SHARED, fd, 0);

	if (plcd == MAP_FAILED)
	{
		perror("mmap fail");
		return -1;
	}
	return 0;
}

/*画点*/
void LCD_Draw_Point(int x, int y, int color)
{

	*(plcd + 800 * y + x) = color;

}

//画一个矩形 左上角坐标x y w h color
void LCD_Draw_matrix(int x, int y, int w, int h, int color)
{
	int i, j;
	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			LCD_Draw_Point(i + x, j + y, color);
		}
	}
}

/*解映射*/
void LCD_UNint()
{
	close(fb_fd);
	munmap(plcd, 800 * 480 * 4);
}

void BMP(string name, int x0, int y0)
{
	const char* n = name.c_str();
	//1.打开Bmp文件
	int fd = open(n, O_RDONLY);
	if (-1 == fd)
	{
		perror("open BMP fail");
		return;
	}

	//2.读取图片的信息 属性 宽  高 色深
	int width, height;
	short depth;
	unsigned char buf[4];

	//移动光标到0x12处 
	lseek(fd, 0x12, SEEK_SET);
	read(fd, buf, 4);
	width = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];

	//移动光标到0x16处 
	lseek(fd, 0x16, SEEK_SET);
	read(fd, buf, 4);
	height = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];

	//移动光标到0x1c处 
	lseek(fd, 0x1c, SEEK_SET);
	read(fd, buf, 2);
	depth = (buf[1] << 8) | buf[0];

	//支持RGB ARGB的bmp图片
	if (!(depth == 24 || depth == 32))
	{

		cout << "NO SURRPORT" << endl;
		return;
	}

	printf("width=%d height=%d depth=%d\n", width, height, depth);

	//3.获取像素数组 然后再屏幕上一一对应的进行显示即可
	//一行有效字节数
	int line_vaild_bytes = abs(width) * depth / 8;
	//一行总字节数=有效字节数+癞子数
	int line_bytes;

	int laizi = 0;//默认赖子为0
	if (line_vaild_bytes % 4)
	{
		laizi = 4 - line_vaild_bytes % 4;
	}
	line_bytes = line_vaild_bytes + laizi;

	//整个像素数组的大小
	int total_bytes = line_bytes * abs(height);

	unsigned char piexl[total_bytes];//装像素
	//读取
	lseek(fd, 54, SEEK_SET);
	read(fd, piexl, total_bytes);

	//只需要在屏幕上显示即可
	//--屏幕ARGB ---
	//24 RGB 补A（透明度）
	//32 ARGB
	unsigned char a, r, g, b;
	int color;
	int i = 0;
	//逐行获取颜色值 
	int x, y;//要显示的那个像素点的坐标
	for (y = 0; y < abs(height); y++)
	{
		//遍历列
		for (x = 0; x < abs(width); x++)
		{
			//小端模式存储 B G R A
			b = piexl[i++];
			g = piexl[i++];
			r = piexl[i++];
			if (depth == 32)
			{
				a = piexl[i++];;
			}
			else
			{
				a = 0;//不透明
			}
			color = a << 24 | r << 16 | g << 8 | b;

			LCD_Draw_Point(width > 0 ? x0 + x : x0 + abs(width) - x - 1,
				height > 0 ? y0 + abs(height) - y - 1 : y0 + y
				, color);
		}
		//跳过
		i += laizi;
	}
	//5.关闭代码
	close(fd);
}