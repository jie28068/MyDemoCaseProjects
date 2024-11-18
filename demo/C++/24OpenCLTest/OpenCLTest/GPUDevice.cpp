#include "GPUDevice.h"
#include <QtGlobal>
#include <immintrin.h>

/**
 * @brief AVX256Write
 * 以256位AVX指令方式对目的内存地址进行写入
 * @param dst 目标地址，必须是32字节对齐
 * @param src 源地址，必须是32字节对齐
 * @param bytes 字节数，必须是32字节整数倍
 */
void AVX256Write(void *dst, const void *src, const size_t bytes)
{
    Q_ASSERT(bytes % 32 == 0);
    Q_ASSERT((intptr_t(src) & 31) == 0);
    Q_ASSERT((intptr_t(dst) & 31) == 0);

    const __m256i *pSrc = reinterpret_cast<const __m256i*>(src);
    __m256i *pDest = reinterpret_cast<__m256i*>(dst);

    int64_t nVects = bytes / sizeof(__m256i);
    for (int64_t i = 0; i < nVects; i++)
    {
        __m256i loaded = _mm256_stream_load_si256(pSrc++);
        _mm256_stream_si256(pDest++, loaded);
    }
    _mm_sfence();
}

/**
 * @brief AVX256Read
 * 以256位AVX指令方式对源内存地址进行读取
 * @param dst 目标地址，必须是32字节对齐
 * @param src 源地址，必须是32字节对齐
 * @param bytes 字节数，必须是32字节整数倍
 */
void AVX256Read(void *dst, void *src ,const size_t bytes)
{
    Q_ASSERT(bytes % 32 == 0);
    Q_ASSERT((intptr_t(src) & 31) == 0);
    Q_ASSERT((intptr_t(dst) & 31) == 0);

    const __m256i *pSrc = reinterpret_cast<const __m256i*>(src);
    __m256i *pDest = reinterpret_cast<__m256i*>(dst);

    int64_t nVects = bytes / sizeof(__m256i);
    for (int64_t i = 0; i < nVects; i++)
    {
        *pDest = _mm256_stream_load_si256(pSrc++);
        pDest++;
    }
}

void bit64Write(void *dst, const void *src, const size_t bytes)
{
    Q_ASSERT(bytes % 8 == 0);
    long long* pSrc = (long long*)src;
    long long* pDst = (long long*)dst;
    for (int i = 0; i < bytes / 8; i++)
    {
        pDst[i] = pSrc[i];
    }
}

void bit64Read(void *dst, void *src ,const size_t bytes)
{
    bit64Write(dst, src, bytes);
}

// ?DMA：CL_MEM_ALLOC_HOST_PTR+CL_MEM_COPY_HOST_PTR
// ?AVX256读写：CL_MEM_READ_WRITE，通过映射来，实现使用AVX256读写。
// ?DIrect：CL_MEM_USE_HOST_PTR
GPUDevice::GPUDevice(cl_device_id *deviceID)
{
    // 创建context
    context = clCreateContext(NULL, 1, deviceID, NULL, NULL, NULL);

    // 创建命令队列
    queue = clCreateCommandQueue(context, *deviceID, CL_QUEUE_PROFILING_ENABLE, NULL);

    // 在GPU设备端申请内存
    // 使用CL_MEM_ALLOC_HOST_PTR标志，opencl可能直接在固定内存上分配内存（而在固定内存上操作系统使用DMA进行数据传输）
    cl_int errCode;
    buffer = clCreateBuffer(context, CL_MEM_ALLOC_HOST_PTR, BufferSize, NULL, &errCode);
}

GPUDevice::~GPUDevice()
{
    clReleaseMemObject(buffer);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}

void GPUDevice::dmaWriteToDevice(const char *data)
{
    // 将data数据拷贝至GPU端内存
    cl_event writeEvent;
    clEnqueueWriteBuffer(queue, buffer, CL_BLOCKING, 0, BufferSize, data, 0, nullptr, &writeEvent);
    //clFlush(queue); // 把command queue中的所有命令提交到device
    clWaitForEvents(1, &writeEvent); // 等待数据传输完成再继续往下执行
}

void GPUDevice::dmaReadFromDevice(char *data)
{
    cl_event readEvent;
    clEnqueueReadBuffer(queue, buffer, CL_BLOCKING, 0, BufferSize, data, 0, nullptr, &readEvent);
    clWaitForEvents(1, &readEvent); // 等待数据传输完成再继续往下执行
}

void GPUDevice::avxWriteToDevice(const char *data)
{
    // 映射内存对象
    void *mappedBuf = clEnqueueMapBuffer(queue, buffer, CL_BLOCKING, CL_MAP_WRITE, 0,
                                         BufferSize, 0, nullptr, nullptr, nullptr);

    AVX256Write(mappedBuf, data, BufferSize);

    // 解除映射
    clEnqueueUnmapMemObject(queue, buffer, mappedBuf, 0, nullptr, nullptr);
}

void GPUDevice::avxReadFromDevice(char *data)
{
    // 映射内存对象
    void *mappedBuf = clEnqueueMapBuffer(queue, buffer, CL_BLOCKING, CL_MAP_READ, 0,
                                         BufferSize, 0, nullptr, nullptr, nullptr);

    AVX256Read(data, mappedBuf, BufferSize);

    // 解除映射
    clEnqueueUnmapMemObject(queue, buffer, mappedBuf, 0, nullptr, nullptr);
}

void GPUDevice::bit64WriteToHost(const char *data)
{
    // 映射内存对象
    void *mappedBuf = clEnqueueMapBuffer(queue, buffer, CL_BLOCKING, CL_MAP_WRITE, 0,
                                         BufferSize, 0, nullptr, nullptr, nullptr);

    bit64Write(mappedBuf, data, BufferSize);

    // 解除映射
    clEnqueueUnmapMemObject(queue, buffer, mappedBuf, 0, nullptr, nullptr);
}

void GPUDevice::bit64ReadFromHost(char *data)
{
    // 映射内存对象
    void *mappedBuf = clEnqueueMapBuffer(queue, buffer, CL_BLOCKING, CL_MAP_WRITE, 0,
                                         BufferSize, 0, nullptr, nullptr, nullptr);

    bit64Read(data, mappedBuf, BufferSize);

    // 解除映射
    clEnqueueUnmapMemObject(queue, buffer, mappedBuf, 0, nullptr, nullptr);
}
