#ifndef FILESAVE_H
#define FILESAVE_H

#include <stdio.h>
#include "RealUCharPort.h"
#include "Mutex.h"

// 最大缓存块个数，以512KB为一个缓存块
#define  BUFBLOCKNUM_MAX    10
// 默认有效缓存块个数，以512KB为一个缓存块
#define  BUFBLOCKNUM_DEF    6
// 一个缓存块的大小
#define BLOCKSIZE           (1 * 512 * 1024)

class FileSave : public RealUCharPort
{
public:
    // 构造函数
    FileSave();
    // 虚构函数
    virtual ~FileSave(void);

    // 开始记录
    bool Start(const char* FileName, unsigned int BlockNum = BUFBLOCKNUM_DEF);
    // 停止记录
    bool Stop();

    // 接收数据函数
    void pushPacket(unsigned char* pPkt,unsigned int iLen);
    // 保存数据线程
    void SaveData();

private:
    // 是否开始记录
    bool m_bStart;

    // 接收缓冲区
    unsigned char* m_RxBuf[BUFBLOCKNUM_MAX];
    unsigned int m_RxLen[BUFBLOCKNUM_MAX];

    // 读、写指针
    unsigned int m_iWr_Block_Ptr;
    unsigned int m_iWr_Ptr;
    unsigned int m_iRd_Block_Ptr;

    // 0-数据到达通知信号量句柄
    // 1-通知线程退出信号量句柄
    HANDLE m_hDataReady[2];

    // 存储线程句柄
    HANDLE m_hSaveThread;

    // 文件
    Mutex m_mFileLock;
    FILE* m_pFile;

    // 实际有效块数
    unsigned int m_iBufBlockSize;
    Mutex m_mBufLock;
    char*    _fileBuf;
};

#endif // FILESAVE_H
