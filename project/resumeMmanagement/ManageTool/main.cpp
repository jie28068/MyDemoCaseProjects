#include "klwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("KeLiang ChangSha");
//    QCoreApplication::setOrganizationDomain("mysoft.com");
    QCoreApplication::setApplicationName("ManageTool");
    a.setStyleSheet("file:///:/styles/klmt.qss");
    KLWidget w;
//    w.show();
    return a.exec();
}
