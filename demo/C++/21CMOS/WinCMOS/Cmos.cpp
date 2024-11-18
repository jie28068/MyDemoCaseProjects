#include "Cmos.h"
#include <stdexcept>
#include "winio.h"

Cmos::Cmos()
{
    if (!InitializeWinIo())
    {
        throw std::runtime_error("InitializeWinIo() is failed!");
    }
}

Cmos::~Cmos()
{
    ShutdownWinIo();
}

/**
 * @brief Cmos::rtcTime
 * 获取RTC时间
 * @return RTC时间
 */
QDateTime Cmos::rtcTime()
{
    // 读取秒
    DWORD s;
    SetPortVal(0x70, 0, 1);
    GetPortVal(0x71, &s, 1);

    // 读取分
    DWORD min;
    SetPortVal(0x70, 2, 1);
    GetPortVal(0x71, &min, 1);

    // 读取时
    DWORD hour;
    SetPortVal(0x70, 4, 1);
    GetPortVal(0x71, &hour, 1);

    // 读取日
    DWORD day;
    SetPortVal(0x70, 7, 1);
    GetPortVal(0x71, &day, 1);

    // 读取月
    DWORD mon;
    SetPortVal(0x70, 8, 1);
    GetPortVal(0x71, &mon, 1);

    // 读取年
    DWORD year;
    SetPortVal(0x70, 9, 1);
    GetPortVal(0x71, &year, 1);

    // 从寄存器中读取到的时间值为BCD码格式，如：hour==0x16表示16点。
    // 需将BCD码格式时间转换为10进制。
    int century = QDate::currentDate().year() / 100 * 100;
    QTime time(bcdToInt(hour), bcdToInt(min), bcdToInt(s));
    QDate date(century + bcdToInt(year), bcdToInt(mon), bcdToInt(day));
    return QDateTime(date, time);
}

/**
 * @brief Cmos::setRtcTime
 * 设置RTC时间，即CMOS实时时钟。
 * @param dateTime 时间
 */
bool Cmos::setRtcTime(const QDateTime &dateTime)
{
    // 每个时间字段需要转换为BCD码格式，如hour==16点，需要转换为0x16进行写入。
    QTime time = dateTime.time();
    QDate date = dateTime.date();

    // 设置秒
    DWORD s = intToBcd(time.second());
    SetPortVal(0x70, 0, 1);
    SetPortVal(0x71, s, 1);

    // 设置分
    DWORD min = intToBcd(time.minute());
    SetPortVal(0x70, 2, 1);
    SetPortVal(0x71, min, 1);

    // 设置时
    DWORD hour = intToBcd(time.hour());
    SetPortVal(0x70, 4, 1);
    SetPortVal(0x71, hour, 1);

    // 设置日
    DWORD day = intToBcd(date.day());
    SetPortVal(0x70, 7, 1);
    SetPortVal(0x71, day, 1);

    // 设置月
    DWORD mon = intToBcd(date.month());
    SetPortVal(0x70, 8, 1);
    SetPortVal(0x71, mon, 1);

    // 设置年
    int century = QDate::currentDate().year() / 100 * 100;
    DWORD year = intToBcd(date.year() - century);
    SetPortVal(0x70, 9, 1);
    SetPortVal(0x71, year, 1);
    return true;
}

/**
 * @brief Cmos::alarmTime
 * 获取RTC alarm时间
 * @return alarm时间
 */
QTime Cmos::alarmTime()
{
    // 读取秒
    DWORD s;
    SetPortVal(0x70, 1, 1);
    GetPortVal(0x71, &s, 1);

    // 读取分
    DWORD min;
    SetPortVal(0x70, 3, 1);
    GetPortVal(0x71, &min, 1);

    // 读取时
    DWORD hour;
    SetPortVal(0x70, 5, 1);
    GetPortVal(0x71, &hour, 1);

    return QTime(bcdToInt(hour), bcdToInt(min), bcdToInt(s));
}

/**
 * @brief Cmos::setAlarmTime
 * 设置RTC alarm时间，即RTC定时开机闹钟。
 * 到达该时间，RTC将会唤醒CPU，启动系统。
 * @param time alarm时间，仅支持24小时以内
 */
bool Cmos::setAlarmTime(const QTime &time)
{
    // 设置秒
    DWORD s = intToBcd(time.second());
    SetPortVal(0x70, 1, 1);
    SetPortVal(0x71, s, 1);

    // 设置分
    DWORD min = intToBcd(time.minute());
    SetPortVal(0x70, 3, 1);
    SetPortVal(0x71, min, 1);

    // 设置时
    DWORD hour = intToBcd(time.hour());
    SetPortVal(0x70, 5, 1);
    SetPortVal(0x71, hour, 1);

    // 使能alarm中断
    DWORD value;
    SetPortVal(0x70, 0x0B, 1);
    GetPortVal(0x71, &value, 1);
    SetPortVal(0x71, value | 0x20, 1);
    return true;
}

/**
 * @brief Cmos::clearAlarmTime
 * 清除RTC alarm时间，即禁用定时开机。
 */
bool Cmos::clearAlarmTime()
{
    // 设置秒
    SetPortVal(0x70, 1, 1);
    SetPortVal(0x71, 0xff, 1);

    // 设置分
    SetPortVal(0x70, 3, 1);
    SetPortVal(0x71, 0xff, 1);

    // 设置时
    SetPortVal(0x70, 5, 1);
    SetPortVal(0x71, 0xff, 1);

    // 禁用alarm中断
    DWORD value;
    SetPortVal(0x70, 0x0B, 1);
    GetPortVal(0x71, &value, 1);
    SetPortVal(0x71, value & (~0x20), 1);
    return true;
}

/**
 * @brief Cmos::bcdToInt
 * BCD码转10进制，如：0x16转换为16
 * @param value 输入值
 * @return 转换结果
 */
int Cmos::bcdToInt(DWORD value)
{
    QString hex = QString("%1").arg(value, 0, 16, QLatin1Char('0'));
    return hex.toInt();
}

/**
 * @brief Cmos::intToBcd
 * 10进制转BCD码，如：16转换为0x16
 * @param value 输入值
 * @return 转换结果
 */
DWORD Cmos::intToBcd(int value)
{
    bool ok;
    QString hex = QString::number(value);
    return hex.toInt(&ok, 16);
}
