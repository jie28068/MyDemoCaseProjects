/*
 * @Author: your name
 * @Date: 2021-10-25 17:11:18
 * @LastEditTime: 2021-10-28 14:50:08
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /桌面/2电子相册/src/main.cpp
 */
#include "Led.h"
#include "touch.h"
#include "pack.h"

int main()
{
    LCD_Init();
    LCD_Draw_matrix(0,0,800,480,GREEN);
   // LCD_Draw_matrix(200,100,200,200,0x00FF00);
    pack();
    LCD_UNint(); 
    
}