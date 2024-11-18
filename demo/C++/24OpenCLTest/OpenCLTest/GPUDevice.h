#ifndef GPUDEVICE_H
#define GPUDEVICE_H

#include <CL/cl.h>

class GPUDevice
{
public:
    GPUDevice(cl_device_id *deviceID);
    ~GPUDevice();

    void dmaWriteToDevice(const char* data);    // 从CPU端通过DMA向GPU端，写入定长数据
    void dmaReadFromDevice(char* data);         // 从GPU端通过DMA向CPU端，读取定长数据

    void avxWriteToDevice(const char* data);    // 使用AVX256指令向GPU端，写入定长数据
    void avxReadFromDevice(char* data);         // 使用AVX256指令从GPU端，读取定长数据

    void bit64WriteToHost(const char* data);    // 在GPU上执行，将定长数据写入CPU端内存
    void bit64ReadFromHost(char* data);         // 在GPU上执行，从CPU端内存读取定长数据

public:
    const static int BufferSize = 128 * 1024;

private:
    cl_context context;
    cl_command_queue queue;
    cl_mem buffer;
};

#endif // GPUDEVICE_H
