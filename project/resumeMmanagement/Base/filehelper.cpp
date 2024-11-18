#include "filehelper.h"


QVariant  FileHelper::GetUser(QString qstrnodename,QString qstrkeyname)
{
    //生成一个ini配置文件
    QSettings  cfg("fzconfig.ini",QSettings::IniFormat);
    //读取分组下指定的值
    QVariant qvar = cfg.value(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname));
    return qvar;

}

/*qstrnodename为分组值，qstrkeyname为'键值'，qvarvalue为'值'*/
void FileHelper::SetUser(QString qstrnodename,QString qstrkeyname,QVariant qvarvalue)
{
    QSettings cfg("fzconfig.ini",QSettings::IniFormat);
    cfg.setValue(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname), qvarvalue);

}

FileHelper::FileHelper()
{

}


void FileHelper::SetResumeSource(QString qstrnodename, QString qstrkeyname, QString qvarvalue)
{
    QSettings cfg("fzconfig.ini",QSettings::IniFormat);

    /*先读取原有ini文件的长度*/
    int i = cfg.beginReadArray(qstrnodename);
    /*读完后要先关闭*/
    cfg.endArray();

    /*开始往里面写内容*/
    cfg.beginWriteArray(qstrnodename);
    /*每调用一次该函数，会增加一条ini字段*/
    cfg.setArrayIndex(i);
    cfg.setValue(qstrkeyname, qvarvalue);
    cfg.endArray();

}

QStringList FileHelper::GetResumeSource(QString qstrnodename, QString qstrkeyname)
{
    QSettings cfg("fzconfig.ini",QSettings::IniFormat);

    int size = cfg.beginReadArray(qstrnodename);

    /*用一个list保存取出来的值*/
    QStringList list;
    for(int i = 0; i < size; ++i)
    {
        /*按下标逐个保存下来*/
        cfg.setArrayIndex(i);
        list.push_back(cfg.value(QString("%1").arg(qstrkeyname)).toString());
    }
    return list;
}

void FileHelper::SetPost(QString qstrnodename, QString qstrkeyname, QVariant qvarvalue)
{
    QSettings cfg("fzconfig.ini",QSettings::IniFormat);

    int i = cfg.beginReadArray(qstrnodename);
    cfg.endArray();

    cfg.beginWriteArray(qstrnodename);
    cfg.setArrayIndex(i);//标记Array的索引
    cfg.setValue(qstrkeyname, qvarvalue);
    cfg.endArray();
}

QStringList FileHelper::GetPost(QString qstrnodename, QString qstrkeyname)
{
    QSettings cfg("fzconfig.ini",QSettings::IniFormat);

    int size = cfg.beginReadArray(qstrnodename);

    QStringList list;
    for(int i = 0; i < size; ++i)
    {
        cfg.setArrayIndex(i);
        list.push_back(cfg.value(QString("%1").arg(qstrkeyname)).toString());
    }
    return list;
}


