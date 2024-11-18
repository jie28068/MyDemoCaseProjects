#include <QCoreApplication>
#include "Platform.h"
#include "GPUDevice.h"

void printDevice(QList<cl_device_id*> deviceIDs)
{
    foreach (cl_device_id* device, deviceIDs)
    {
        char name[128] = {0};
        char ext_data[128] = {0};
        char devtype[128] = {0};

        clGetDeviceInfo(*device, CL_DEVICE_NAME,
            sizeof(name), name, NULL);

        clGetDeviceInfo(*device, CL_DEVICE_EXTENSIONS,
            sizeof(ext_data), ext_data, NULL);

        clGetDeviceInfo(*device, CL_DEVICE_TYPE,
            sizeof(devtype), devtype, NULL);

        printf("NAME: %s\nEXTENSIONS: %s\nDevice Type:%d\n",
            name, ext_data, devtype[0]);
    }
}

char* createTestData()
{
    char* srcData = new char[GPUDevice::BufferSize];
    for (int i = 0; i < GPUDevice::BufferSize; i++)
    {
        srcData[i] = i % 128;
    }
    return srcData;
}

char* createAlignTestData()
{
    char* srcData = (char*)_aligned_malloc(GPUDevice::BufferSize, 32);
    for (int i = 0; i < GPUDevice::BufferSize; i++)
    {
        srcData[i] = i % 128;
    }
    return srcData;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 打印当前GPU设备信息
    Platform platform;
    QMap<QString, cl_device_id*> deviceIDs = platform.getSupportGPUDevices();
    printDevice(deviceIDs.values());

    // 1.测试DMA方式读写GPU内存
    char* buf1 = createTestData();
    GPUDevice gpuDevice(deviceIDs.values().at(0));
    gpuDevice.dmaWriteToDevice(buf1);

    char* buf2 = new char[GPUDevice::BufferSize];
    gpuDevice.dmaReadFromDevice(buf2);
    if (memcmp(buf1, buf2, GPUDevice::BufferSize) == 0)
    {
        printf("dma verify is ok\n");
    }
    else
    {
        printf("dma verify is error\n");
    }
    delete[] buf1;
    delete[] buf2;

    // 2.测试AVX256读写GPU内存
    char* buf3 = createAlignTestData();
    gpuDevice.avxWriteToDevice(buf3);

    char* buf4 = (char*)_aligned_malloc(GPUDevice::BufferSize, 32);
    gpuDevice.avxReadFromDevice(buf4);
    if (memcmp(buf3, buf4, GPUDevice::BufferSize) == 0)
    {
        printf("avx verify is ok\n");
    }
    else
    {
        printf("avx verify is error\n");
    }
    _aligned_free(buf3);
    _aligned_free(buf4);

    // 3.测试64bit读写主机内存
    char* buf5 = createAlignTestData();
    gpuDevice.bit64WriteToHost(buf5);

    char* buf6 = (char*)_aligned_malloc(GPUDevice::BufferSize, 32);
    gpuDevice.bit64ReadFromHost(buf6);
    if (memcmp(buf5, buf6, GPUDevice::BufferSize) == 0)
    {
        printf("bit64 verify is ok\n");
    }
    else
    {
        printf("bit64 verify is error\n");
    }
    _aligned_free(buf5);
    _aligned_free(buf6);

    return a.exec();
}


