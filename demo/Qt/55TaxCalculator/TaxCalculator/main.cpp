#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QMessageBox>
#include "UAC.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 以管理员权限启动一个新实例
    if (UAC::runAsAdmin())
    {
        return 0; // 启动成功，当前程序退出
    } // 未启动，当前程序继续

    // 加载语言文件
    QTranslator translator;
    if (translator.load("tax_zh", QApplication::applicationDirPath() + "/language"))
    {
        // 安装语言文件
        if (!QApplication::installTranslator(&translator))
        {
            QMessageBox::warning(nullptr, "Warning", "Failed to install the language file");
        }
    }
    else
    {
        QMessageBox::warning(nullptr, "Warning", "The language file failed to load");
    }

    MainWindow w;
    w.show();

    return a.exec();
}
