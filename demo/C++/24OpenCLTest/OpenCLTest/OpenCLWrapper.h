#ifndef OPENCLWRAPPER_H
#define OPENCLWRAPPER_H

#include <CL/cl.h>

class OpenCLWrapper
{
public:
    OpenCLWrapper(cl_device_id *device);
    ~OpenCLWrapper();

    void dmaWriteToDevice(const char* data); // 写入定长数据
    void dmaReadFromDevice(char* data);  // 读取定长数据

public:
    const static int BufferSize = 128 * 1024;

private:
    cl_context context;
    cl_command_queue queue;
    cl_mem buffer;
};

#endif // OPENCLWRAPPER_H
