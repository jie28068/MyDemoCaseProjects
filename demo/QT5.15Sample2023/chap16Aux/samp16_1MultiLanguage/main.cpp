#include "mainwindow.h"
#include    <QApplication>
#include    <QTranslator>

#include    <QSettings>
#include    <QMessageBox>

QTranslator *trans=NULL;

QString readSetting();

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    trans=new QTranslator;
    QString curLang=readSetting(); //读取语言设置
    bool ok=false;
    if (curLang=="EN")
        ok= trans->load("samp16_1_en.qm");
    else
        ok= trans->load("samp16_1_cn.qm");
    if (ok)
        app.installTranslator(trans);
    else
       qDebug("载入ts文件错误");

    MainWindow w;
    w.show();
    return app.exec();
}

QString readSetting()
{//从注册表读取上次设置的语言
    QString organization="WWB-Qt";//用于注册表，
    QString appName="samp16_1"; //HKEY_CURRENT_USER/WWB-Qt/amp13_1
    QSettings  settings(organization,appName);//创建
    QString Language=settings.value("Language","EN").toString();//读取 saved键的值
    return  Language;
}
