#ifndef PRIZECONTROL_H
#define PRIZECONTROL_H

// 奖项
enum Prize
{
    First,     // 一等奖
    Second,    // 二等奖
    Third,     // 三等奖
    Fourth,    // 四等奖
    Thanks     // 谢谢参与
};

// 抽奖控制类
class PrizeControl
{
public:
    PrizeControl();

    // 抽奖，返回抽奖结果
    Prize drawLottery();
};

#endif // PRIZECONTROL_H
