#include <QCoreApplication>
#include "UAC.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 以管理员权限启动一个新实例
    if (UAC::runAsAdmin())
    {
        return 0; // 启动成功，当前程序退出
    } // 未启动，当前程序继续

    return a.exec();
}
