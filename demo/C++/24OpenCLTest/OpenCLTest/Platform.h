#ifndef PLATFORM_H
#define PLATFORM_H

#include <CL/cl.h>
#include <QMap>

class Platform
{
public:
    Platform();
    ~Platform();

    QMap<QString, cl_device_id*> getSupportGPUDevices();
    cl_device_id* isExistGPUDevice(const QString& name);

private:
    cl_platform_id* platforms;
    QMap<QString, cl_device_id*> devices;
};

#endif // PLATFORM_H
