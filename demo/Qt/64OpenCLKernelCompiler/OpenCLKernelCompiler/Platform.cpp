#include "Platform.h"
#include <QDebug>

Platform::Platform()
{
    // 获取可用OpenCL平台数量
    cl_uint platformCount;
    cl_int ret = clGetPlatformIDs(0, nullptr, &platformCount);
    if (ret != CL_SUCCESS)
    {
        qDebug() << "Error: clGetPlatformIDs failed. Error code : " << ret;
        return;
    }

    // 获取所有平台对象
    cl_platform_id* platforms = new cl_platform_id[platformCount];
    ret = clGetPlatformIDs(platformCount, platforms, nullptr);
    if (ret != CL_SUCCESS)
    {
        delete[] platforms;
        platforms = nullptr;
        qDebug() << "Error: clGetPlatformIDs failed. Error code : " << ret;
        return;
    }

    // 遍历每个平台，获取所有的GPU设备
    for (int i = 0; i < platformCount; i++)
    {
        // 首先获取该平台下，有多少个GPU设备
        cl_uint deviceCount;
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU, 0, nullptr, &deviceCount);
        if (deviceCount == 0)
        {
            continue;
        }

        // 分配该数量的设备对象内存空间
        cl_device_id* device = new cl_device_id[deviceCount];

        // 获取该平台下，所有GPU设备对象
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU, deviceCount, device, NULL);

        // 遍历每个设备的名称
        DeviceMap deviceMap;
        for(int i = 0; i < deviceCount; i++)
        {
            char name[256] = {0};
            clGetDeviceInfo(device[i], CL_DEVICE_NAME, sizeof(name), name, NULL);
            deviceMap[name] = device[i];
        }

        // 获取平台名称
        char platformName[256] = {0};
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(platformName), platformName, NULL);
        platformMap[platformName] = deviceMap;

        delete[] device;
    }

    delete[] platforms;
}

Platform::~Platform()
{

}

QStringList Platform::getPlatformNames()
{
    return platformMap.keys();
}

QStringList Platform::getGPUDeviceNames(const QString &platformName)
{
    return platformMap.value(platformName).keys();
}

cl_device_id Platform::getGPUDeviceID(const QString &platformName, const QString &deviceName)
{
    DeviceMap deviceMap = platformMap.value(platformName);
    if (deviceMap.contains(deviceName))
    {
        return deviceMap[deviceName];
    }
    else
    {
        return nullptr;
    }
}
