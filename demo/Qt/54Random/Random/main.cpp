#include <QCoreApplication>
#include <QDebug>
#include "Random.h"
#include "CTimer.h"

void test_c_random()
{
    srand(time(nullptr));

    CTimer timer;
    timer.reset();  // 开始计时

    for (int i = 0; i < 200; i++)
    {
        int val = rand() % 20;
    }

    double elapsed = timer.end();  // 结束计时
    qDebug() << "elapsed time:" << elapsed << "us";
}

void test_qt_random()
{
    qsrand(time(nullptr));

    CTimer timer;
    timer.reset();  // 开始计时

    for (int i = 0; i < 200; i++)
    {
        int val = qrand() % 20;
    }

    double elapsed = timer.end();  // 结束计时
    qDebug() << "elapsed time:" << elapsed << "us";
}

void test_custom_random()
{
    Random random(time(nullptr));

    CTimer timer;
    timer.reset();  // 开始计时

    for (int i = 0; i < 200; i++)
    {
        int val = random.rand32() % 20;
    }

    double elapsed = timer.end();  // 结束计时
    qDebug() << "elapsed time:" << elapsed << "us";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    test_c_random();
    test_qt_random();
    test_custom_random();

    return a.exec();
}
