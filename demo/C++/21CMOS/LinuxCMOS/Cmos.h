#ifndef CMOS_H
#define CMOS_H

#include <QDateTime>

/**
 * @brief The Cmos class
 * linux下CMOS寄存器操作，包括获取/设置rtc、alarm时间。
 * 该类中涉及日期时间均为UTC时间，非本地时间。
 */
class Cmos
{
public:
    Cmos();
    ~Cmos();

    QDateTime rtcTime();
    bool setRtcTime(const QDateTime& dateTime);

    QTime alarmTime();
    bool setAlarmTime(const QTime& time);
    bool clearAlarmTime();

private:
    int fd;
};

#endif // CMOS_H
