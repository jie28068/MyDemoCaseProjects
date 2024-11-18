#ifndef PLATFORM_H
#define PLATFORM_H

#include <CL/cl.h>
#include <QStringList>
#include <QMap>

/**
 * @brief The Platform class
 * OpenCL平台类，实现获取当前系统支持的平台和设备
 */
class Platform
{
public:
    Platform();
    ~Platform();
    QStringList getPlatformNames();
    QStringList getGPUDeviceNames(const QString& platformName);
    cl_device_id getGPUDeviceID(const QString& platformName, const QString& deviceName);

private:
    typedef QMap<QString, cl_device_id> DeviceMap;
    QMap<QString, DeviceMap> platformMap;
};

#endif // PLATFORM_H
