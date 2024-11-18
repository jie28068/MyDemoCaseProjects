#include <QCoreApplication>
#include "QBreakpadHandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QBreakpadInstance.setDumpPath("crashes"); // 设置生成dump文件路径

    // 执行此句发生异常时，会自动生成dump文件
    *((int*)0) = 10;

    return a.exec();
}
