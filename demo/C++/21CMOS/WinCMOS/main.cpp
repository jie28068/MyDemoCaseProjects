#include <QCoreApplication>
#include <QDebug>
#include "Cmos.h"

void test()
{
    try
    {
        /* 1.测试读写RTC时间 */
        // 读取当前rtc时间
        Cmos cmos;
        QDateTime curDateTime = cmos.rtcTime();
        qDebug() << "current rtc time:" << curDateTime;

        // 设置一个新的rtc时间
        QDateTime newDateTime = curDateTime.addDays(-2);
        cmos.setRtcTime(newDateTime);
        qDebug() << "new rtc time:" << cmos.rtcTime();

        // 恢复rtc时间
        cmos.setRtcTime(QDateTime::currentDateTime());
        qDebug() << "restore rtc time:" << cmos.rtcTime();

        /* 2.测试读写alarm时间 */
        // 读取当前alarm时间
        qDebug() << "current alarm time:" << cmos.alarmTime();

        // 设置一个新的alarm时间
        cmos.setAlarmTime(QDateTime::currentDateTime().addSecs(120).time());
        qDebug() << "new alarm time:" << cmos.alarmTime();

        // 清除alarm时间
        cmos.clearAlarmTime();
        qDebug() << "clear alarm time:" << cmos.alarmTime();
    }
    catch (const std::exception& e)
    {
        qDebug() << e.what();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    test();

    return a.exec();
}
