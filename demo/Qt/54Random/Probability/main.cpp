#include <QCoreApplication>
#include <QDebug>
#include "PrizeControl.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int firstCount = 0;     // 一等奖抽中次数
    int secondCount = 0;    // 二等奖抽中次数
    int thirdCount = 0;     // 三等奖抽中次数
    int fourthCount = 0;    // 四等奖抽中次数
    int thanksCount = 0;    // 谢谢参与抽中次数

    PrizeControl prizeControl;
    for (int i = 0; i < 2000; i++) // 总共抽奖次数2000
    {
        Prize prize = prizeControl.drawLottery();
        switch (prize)
        {
        case First:
            firstCount++;
            break;
        case Second:
            secondCount++;
            break;
        case Third:
            thirdCount++;
            break;
        case Fourth:
            fourthCount++;
            break;
        default:
            thanksCount++;
            break;
        }
    }

    qDebug() << "firstCount:" << firstCount;    //100
    qDebug() << "secondCount:" << secondCount;  //200
    qDebug() << "thirdCount:" << thirdCount;    //400
    qDebug() << "fourthCount:" << fourthCount;  //600
    qDebug() << "thanksCount:" << thanksCount;  //700

    return a.exec();
}
