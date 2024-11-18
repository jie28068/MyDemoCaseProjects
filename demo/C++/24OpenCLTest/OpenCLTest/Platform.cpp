#include "Platform.h"

Platform::Platform()
    : platforms(nullptr)
{
    // 获取可用OpenCL平台数量
    cl_uint platformCount;
    cl_int ret = clGetPlatformIDs(0, nullptr, &platformCount);
    if (ret < 0 || platformCount == 0)
    {
        return;
    }

    // 分配该数量的平台对象内存空间
    platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * platformCount);

    // 获取平台对象
    clGetPlatformIDs(platformCount, platforms, NULL);

    // 遍历每个平台，获取所有的GPU设备
    for (int i = 0; i < platformCount; i++)
    {
        // 首先获取该平台下，有多少个GPU设备
        cl_uint deviceCount;
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 0, NULL, &deviceCount);
        if (deviceCount == 0)
        {
            continue;
        }

        // 分配该数量的设备对象内存空间
        cl_device_id* device = (cl_device_id*)malloc(sizeof(cl_device_id) * deviceCount);

        // 获取该平台下，所有GPU设备对象
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, deviceCount, device, NULL);

        // 遍历每个设备的名称
        for(int i = 0; i < deviceCount; i++)
        {
            char name_data[256] = {0};
            clGetDeviceInfo(device[i], CL_DEVICE_NAME, sizeof(name_data), name_data, NULL);
            devices[name_data] = &device[i];
        }
    }
}

Platform::~Platform()
{
    // 释放GPU对象
    foreach (auto name, devices.keys())
    {
        free(devices[name]);
    }
    devices.clear();

    // 释放平台对象内存空间
    if (platforms)
    {
        free(platforms);
        platforms = nullptr;
    }
}

QMap<QString, cl_device_id*> Platform::getSupportGPUDevices()
{
    return devices;
}

cl_device_id *Platform::isExistGPUDevice(const QString &name)
{
    return devices.value(name, nullptr);
}
