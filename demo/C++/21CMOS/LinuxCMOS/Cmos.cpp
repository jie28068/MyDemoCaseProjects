#include "Cmos.h"
#include <stdexcept>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

Cmos::Cmos()
{
    // 打开RTC设备
    fd = open("/dev/rtc0", O_RDONLY);
    if (fd < 0)
    {
        throw std::runtime_error("/dev/rtc0:device open failed");
    }
}

Cmos::~Cmos()
{
    close(fd);
}

/**
 * @brief Cmos::rtcTime
 * 获取RTC时间
 * @return RTC时间
 */
QDateTime Cmos::rtcTime()
{
    // 读取当前RTC时间
    struct rtc_time rtc_tm;
    if (ioctl(fd, RTC_RD_TIME, &rtc_tm) < 0)
    {
        return QDateTime();
    }

    QDate date(rtc_tm.tm_year + 1900, rtc_tm.tm_mon + 1, rtc_tm.tm_mday);
    QTime time(rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
    return QDateTime(date, time, Qt::UTC);
}

/**
 * @brief Cmos::setRtcTime
 * 设置RTC时间，即CMOS实时时钟。
 * @param dateTime 时间
 */
bool Cmos::setRtcTime(const QDateTime &dateTime)
{
    struct rtc_time rtc_tm;
    rtc_tm.tm_year = dateTime.date().year() - 1900;
    rtc_tm.tm_mon = dateTime.date().month() - 1;
    rtc_tm.tm_mday = dateTime.date().day();
    rtc_tm.tm_hour = dateTime.time().hour();
    rtc_tm.tm_min = dateTime.time().minute();
    rtc_tm.tm_sec = dateTime.time().second();
    if (ioctl(fd, RTC_SET_TIME, &rtc_tm) < 0)
    {
        return false;
    }
    return true;
}

/**
 * @brief Cmos::alarmTime
 * 获取RTC alarm时间
 * @return alarm时间
 */
QTime Cmos::alarmTime()
{
    struct rtc_time rtc_tm;
    if (ioctl(fd, RTC_ALM_READ, &rtc_tm) < 0)
    {
        return QTime();
    }
    return QTime(rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
}

/**
 * @brief Cmos::setAlarmTime
 * 设置RTC alarm时间，即RTC定时开机闹钟。
 * 到达该时间，RTC将会唤醒CPU，启动系统。
 * @param time alarm时间，仅支持24小时以内
 */
bool Cmos::setAlarmTime(const QTime &time)
{
    struct rtc_time rtc_tm;
    rtc_tm.tm_hour = time.hour();
    rtc_tm.tm_min = time.minute();
    rtc_tm.tm_sec = time.second();
    if (ioctl(fd, RTC_ALM_SET, &rtc_tm) < 0)
    {
        return false;
    }

    // 使能alarm中断
    if (ioctl(fd, RTC_AIE_ON, 0) < 0)
    {
        return false;
    }
    return true;
}

/**
 * @brief Cmos::clearAlarmTime
 * 清除RTC alarm时间，即禁用定时开机。
 */
bool Cmos::clearAlarmTime()
{
    struct rtc_time rtc_tm;
    rtc_tm.tm_hour = 0;
    rtc_tm.tm_min = 0;
    rtc_tm.tm_sec = 0;
    if (ioctl(fd, RTC_ALM_SET, &rtc_tm) < 0)
    {
        return false;
    }

    // 禁用alarm中断
    if (ioctl(fd, RTC_AIE_OFF, 0) < 0)
    {
        return false;
    }
    return true;
}
