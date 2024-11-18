#include <QCoreApplication>
#include <queue>
#include <QDebug>
#include "IORequestQueue.h"
#include "CTimer.h"

void testStdQueue(std::vector<IORequest>& requests)
{
    /***********************测试入队*************************/
    CTimer timer;
    timer.reset();
    std::queue<IORequest*> stdQueue;
    for (unsigned int i = 0; i < requests.size(); i++) // 将IO请求添加到std::queue队列中
    {
        stdQueue.push(&requests[i]);
    }
    double elapsed = timer.end();
    qDebug() << "std::queue push:" << elapsed << "us";

    /***********************测试出队*************************/
    timer.reset();
    for (unsigned int i = 0; i < requests.size(); i++) // 从std::queue出队列
    {
        IORequest* req = stdQueue.front(); // 返回队头元素
        stdQueue.pop(); // 将队头元素弹出
    }
    elapsed = timer.end();
    qDebug() << "std::queue pop:" << elapsed << "us";
}

void testIOQueue(std::vector<IORequest>& requests)
{
    /***********************测试入队*************************/
    CTimer timer;
    timer.reset();
    IORequestQueue ioQueue;
    for (unsigned int i = 0; i < requests.size(); i++) // 将IO请求添加到IORequestQueue队列中
    {
        ioQueue.push(&requests[i]);
    }
    double elapsed = timer.end();
    qDebug() << "IORequestQueue push:" << elapsed << "us";

    /***********************测试出队*************************/
    timer.reset();
    for (unsigned int i = 0; i < requests.size(); i++) // 从IORequestQueue出队列
    {
        IORequest* req = ioQueue.front(); // 返回队头元素
        ioQueue.pop(); // 将队头元素弹出
    }
    elapsed = timer.end();
    qDebug() << "IORequestQueue pop:" << elapsed << "us";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 准备测试元素10000个
    std::vector<IORequest> requests;
    for (unsigned int index = 0; index < 10000; index++)
    {
        IORequest req;
        req.data = 0;
        req.ioType = 0;
        req.requestIndex = index;
        requests.push_back(req);
    }

    testStdQueue(requests); // 测试std::queue
    testIOQueue(requests);  // 测试IORequestQueue

    return a.exec();
}
