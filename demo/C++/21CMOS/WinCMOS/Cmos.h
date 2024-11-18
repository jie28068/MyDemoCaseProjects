#ifndef CMOS_H
#define CMOS_H

#include <QDateTime>
#include <Windows.h>

/**
 * @brief The Cmos class
 * win下CMOS寄存器操作，包括获取/设置rtc、alarm时间。
 * 该类中涉及日期时间均为本地时间，非UTC时间。
 * 本类依赖于WinIO三方库，使用时有如下限制：
 * 1.在cmd中使用命令"bcdedit.exe /set TESTSIGNING ON"开启测试模式，
 * 然后重启系统，进入win测试模式。
 * 2.安装WinIo64.sys的测试签名。
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
    int bcdToInt(DWORD value);
    DWORD intToBcd(int value);
};

#endif // CMOS_H
