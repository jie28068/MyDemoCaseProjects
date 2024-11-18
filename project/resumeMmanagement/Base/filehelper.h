#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QObject>

#include <QCoreApplication>
#include <QSettings>
#include <QVariant>
#include <QDebug>

/*该接口记录上次操作内容*/
class FileHelper
{
public:
    /*账号密码*/
    static QVariant GetUser(QString qstrnodename,QString qstrkeyname);
    static void		SetUser(QString qstrnodename,QString qstrkeyname,QVariant qvarvalue);
    FileHelper();

    /*简历来源*/
    static void SetResumeSource(QString qstrnodename,QString qstrkeyname,QString qvarvalue);
    static QStringList GetResumeSource(QString qstrnodename,QString qstrkeyname);

    /*岗位*/
    static void SetPost(QString qstrnodename,QString qstrkeyname,QVariant qvarvalue);
    static QStringList GetPost(QString qstrnodename,QString qstrkeyname);
};

#endif // FILEHELPER_H
