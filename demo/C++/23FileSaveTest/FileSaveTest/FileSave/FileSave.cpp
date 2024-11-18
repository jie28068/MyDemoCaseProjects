#include "FileSave.h"
#include <QDebug>
#include <QMessageBox>

DWORD WINAPI SaveDataThread(LPVOID lpParameter)
{
    FileSave* pSave = (FileSave*)lpParameter;

    pSave->SaveData();

    return(0);
}

FileSave::FileSave()
    : m_pFile(NULL),
      m_bStart(false),
      m_iBufBlockSize(0),
      m_iWr_Block_Ptr(0),
      m_iWr_Ptr(0),
      m_iRd_Block_Ptr(0),
      _fileBuf(NULL)
{
    //缓冲区初始化
    for(int i = 0; i < BUFBLOCKNUM_MAX; i++)
    {
        m_RxBuf[i] = NULL;
        m_RxLen[i] = 0;
    }

    //初始化句柄
    m_hDataReady[0] = NULL;
    m_hDataReady[1] = NULL;
    m_hSaveThread = NULL;

    //创建信号量
    m_hDataReady[0] = CreateSemaphore(NULL, 0, BUFBLOCKNUM_MAX, NULL);
    if(m_hDataReady[0] == NULL)
    {
        return;
    }
    m_hDataReady[1] = CreateSemaphore(NULL, 0, 1, NULL);
    if(m_hDataReady[1] == NULL)
    {
        CloseHandle(m_hDataReady[0]);
        m_hDataReady[0] = NULL;
        return;
    }

    //创建一个线程用于保存文件
    m_hSaveThread = CreateThread(NULL, 0, SaveDataThread, this, 0, NULL);
    if(m_hSaveThread == NULL)
    {
        CloseHandle(m_hDataReady[0]);
        m_hDataReady[0] = NULL;
        CloseHandle(m_hDataReady[1]);
        m_hDataReady[1] = NULL;
        return;
    }
}

FileSave::~FileSave(void)
{
    if(m_hSaveThread != NULL)
    {
        //发出线程退出通知事件
        ReleaseSemaphore(m_hDataReady[1], 1, NULL);
        //等待线程正常退出(最多等待3000ms)
        if(WAIT_TIMEOUT == WaitForSingleObject(m_hSaveThread, 3000))
        {
            TerminateThread(m_hSaveThread, 0); // 如果线程超时未退出，强制终止线程
        }

        //关闭句柄
        CloseHandle(m_hDataReady[0]);
        m_hDataReady[0] = NULL;
        CloseHandle(m_hDataReady[1]);
        m_hDataReady[1] = NULL;
        CloseHandle(m_hSaveThread);
        m_hSaveThread = NULL;
    }

    //关闭文件
    if(m_bStart)
    {
        Stop();
        m_bStart = false;
    }
}

bool FileSave::Start(const char* FileName, unsigned int BlockNum)
{
    //已经开始了，不得再次开始
    if(m_bStart)
        return false;

    if(FileName == NULL)
        return false;

    //打开文件
    m_pFile = fopen(FileName, "wb+");
    if (m_pFile == NULL)
        return false;

    //申请缓冲空间
    try
    {
        _fileBuf = new char[BLOCKSIZE];
        for(unsigned int i = 0; i < BlockNum; i++)
        {
            m_RxBuf[i] = new unsigned char[BLOCKSIZE];
            m_RxLen[i] = 0;
        }
    }
    catch(...)
    {
        for(unsigned int i = 0; i < BlockNum; i++)
        {
            if (m_RxBuf[i] != NULL)
            {
                delete m_RxBuf[i];
                m_RxBuf[i] = NULL;
                m_RxLen[i] = 0;
            }
        }
        if (_fileBuf != NULL)
        delete _fileBuf;
        _fileBuf = NULL;

        fclose(m_pFile);
        m_pFile = NULL;

        QMessageBox::warning(NULL, QObject::tr("warning"), QObject::tr("Memory request failed"));
        return false;
    }

    setvbuf(m_pFile, _fileBuf, _IOFBF, BLOCKSIZE);

    for(int i = BlockNum; i < BUFBLOCKNUM_MAX; i++)
    {
        m_RxBuf[i] = NULL;
        m_RxLen[i] = 0;
    }
    m_iBufBlockSize = BlockNum;

    m_iWr_Block_Ptr = 0;
    m_iWr_Ptr = 0;
    m_iRd_Block_Ptr = m_iBufBlockSize - 1;

    m_bStart = true;
    return true;
}

bool FileSave::Stop()
{
    if(!m_bStart) return false;

    m_bStart = false;

    m_mFileLock.lock();

    fclose(m_pFile);
    m_pFile = NULL;

    delete _fileBuf;
    _fileBuf = NULL;

    m_mFileLock.unlock();

    //释放原有缓存空间
    for(int i = 0; i < BUFBLOCKNUM_MAX; i++)
    {
        if(m_RxBuf[i] != NULL)
        {
            delete []m_RxBuf[i];
        }
        m_RxBuf[i] = NULL;
        m_RxLen[i] = 0;
    }
    m_iBufBlockSize = 0;

    return true;
}

void FileSave::pushPacket(unsigned char* pPkt,unsigned int iLen)
{
    if(!m_bStart) return;

    m_mBufLock.lock();

    if((m_iWr_Ptr+iLen) > BLOCKSIZE)
    {
        m_RxLen[m_iWr_Block_Ptr] = m_iWr_Ptr;
        ReleaseSemaphore(m_hDataReady[0], 1, NULL);
        m_iWr_Ptr = 0;
        m_iWr_Block_Ptr = (m_iWr_Block_Ptr + 1) % m_iBufBlockSize;

        //m_mFileLock.Lock();
        if(m_iWr_Block_Ptr == m_iRd_Block_Ptr)
        {
            //TRACE("缓冲区溢出:Wr=%d,Rd=%d\n",m_iWr_Block_Ptr,m_iRd_Block_Ptr);
            //qDebug("缓冲区溢出:Wr=%d,Rd=%d\n",m_iWr_Block_Ptr,m_iRd_Block_Ptr);
        }
        //m_mFileLock.Unlock();
    }

    memcpy(m_RxBuf[m_iWr_Block_Ptr] + m_iWr_Ptr, pPkt, iLen);
    m_iWr_Ptr += iLen;

    m_mBufLock.unlock();
}

void FileSave::SaveData()
{
    while(true)
    {
        //等待事件
        DWORD dwEvent = WaitForMultipleObjects(2, m_hDataReady, FALSE, 1000);
        if (dwEvent == WAIT_OBJECT_0 + 0)  // 数据到达
        {
            //保存数据到文件
            if(m_bStart)
            {
                // 获取读地址与数据长度
                m_mBufLock.lock();

                m_iRd_Block_Ptr = (m_iRd_Block_Ptr + 1) % m_iBufBlockSize;

                //存盘
                //m_mFileLock.lock();
                fwrite(m_RxBuf[m_iRd_Block_Ptr], m_RxLen[m_iRd_Block_Ptr], 1, m_pFile);
                //fflush(m_pFile);
                //m_mFileLock.unlock();

                m_mBufLock.unlock();
            }
        }
        else if(dwEvent == WAIT_OBJECT_0 + 1)  // 线程退出
        {
            break;
        }
        else if (dwEvent == WAIT_TIMEOUT)  // 超时
        {
            if(m_bStart)
            {
                m_mBufLock.lock();

                if(m_iWr_Ptr != 0)
                {
                    m_RxLen[m_iWr_Block_Ptr] = m_iWr_Ptr;
                    ReleaseSemaphore(m_hDataReady[0], 1, NULL);
                    m_iWr_Ptr = 0;
                    m_iWr_Block_Ptr = (m_iWr_Block_Ptr + 1) % m_iBufBlockSize;
                }

                m_mBufLock.unlock();
            }
        }
    }
}
