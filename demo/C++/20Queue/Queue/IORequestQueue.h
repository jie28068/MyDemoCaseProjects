#ifndef IOREQUESTQUEUE_H
#define IOREQUESTQUEUE_H

#include <assert.h>

struct IORequest
{
    IORequest* p;
    int data;
    int ioType;
    unsigned int requestIndex;
};

class IORequestQueue
{
public:
    IORequestQueue()
        : pHead(nullptr),
          pTail(nullptr),
          count(0)
    {}

    // 队列是否为空
    bool empty() const
    {
        return (pHead == nullptr);
    }

    // 返回队列中元素个数
    size_t size() const
    {
        return count;
    }

    // 返回队头元素
    IORequest* front()
    {
        assert(!empty());
        return pHead;
    }

    // 返回队尾元素
    IORequest* back()
    {
        assert(!empty());
        return pTail;
    }

    // 将变量request从队列尾入队
    void push(IORequest* request)
    {
        request->p = nullptr;
        if (pHead == nullptr)
        {
            assert(pTail == nullptr);
            pHead = request;
        }
        else
        {
            assert(pTail != nullptr);
            pTail->p = request;
        }
        pTail = request;
        count++;
    }

    // 将队头元素弹出
    void pop()
    {
        assert(!empty());
        pHead = pHead->p;
        if (pHead == nullptr)
        {
            pTail = nullptr;
        }
        count--;
    }

private:
    IORequest* pHead;
    IORequest* pTail;
    size_t count;
};

#endif // IOREQUESTQUEUE_H
