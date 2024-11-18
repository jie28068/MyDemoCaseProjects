#include <QCoreApplication>
#include "my1.h"
#include "my2.h"
#include "QBreakpadHandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QBreakpadInstance.setDumpPath("crashes"); // 设置生成dump文件路径

    My1 xx;
    xx.add(1, 2);

    My2 yy;
    yy.sub(2, 1);

    return a.exec();
}
