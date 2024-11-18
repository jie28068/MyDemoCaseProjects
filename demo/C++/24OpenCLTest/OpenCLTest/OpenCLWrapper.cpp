#include "OpenCLWrapper.h"

OpenCLWrapper::OpenCLWrapper(cl_device_id *device)
{
    // 创建context
    context = clCreateContext(NULL, 1, device, NULL, NULL, NULL);

    // 创建命令队列
    queue = clCreateCommandQueue(context, *device, CL_QUEUE_PROFILING_ENABLE, NULL);

    // 在GPU设备端申请内存
    // 使用CL_MEM_ALLOC_HOST_PTR标志，opencl可能直接在固定内存上分配内存（而在固定内存上操作系统使用DMA进行数据传输）
    cl_int errCode;
    buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR, BufferSize, NULL, &errCode);
}

OpenCLWrapper::~OpenCLWrapper()
{
    clReleaseMemObject(buffer);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}

void OpenCLWrapper::dmaWriteToDevice(const char *data)
{
    // 将data数据拷贝至GPU端内存
    cl_event writeEvt;
    clEnqueueWriteBuffer(queue, buffer, 1, 0, BufferSize, data, 0, 0, 0);
    //clFlush(queue); // 把command queue中的所有命令提交到device
    clWaitForEvents(1, &writeEvt); // 等待数据传输完成再继续往下执行
}

void OpenCLWrapper::dmaReadFromDevice(char *data)
{

}
