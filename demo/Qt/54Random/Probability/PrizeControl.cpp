#include "PrizeControl.h"
#include <time.h>
#include <stdlib.h>

PrizeControl::PrizeControl()
{
    srand(time(nullptr)); // 设置随机数种子
}

Prize PrizeControl::drawLottery()
{
    // 生成一个1 ~ 100范围内的随机数
    int value = rand() % 100 + 1; // 1 ~ 100

    if (value >= 1 && value <= 5)
    {
        return First; // 一等奖5%
    }
    else if (value > 5 && value <= 15)
    {
        return Second; // 二等奖10%
    }
    else if (value > 15 && value <= 35)
    {
        return Third; // 三等奖20%
    }
    else if (value > 35 && value <= 65)
    {
        return Fourth; // 四等奖30%
    }
    else
    {
        return Thanks; // 谢谢参与35%
    }
}
