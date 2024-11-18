#ifndef SIGNTOOL_H
#define SIGNTOOL_H
#include <QHash>
#include <QCryptographicHash>
#include <QString>
#include <QByteArray>

class SignTool{
public:
    SignTool(){};

    //数据流签名，永久签名，算法如有更改，数据库内相关数据要重新签名
    static QString SignForStream(QByteArray bytes){
        return QCryptographicHash::hash(bytes,QCryptographicHash::Md5).toHex().toUpper();
    };
    //密码签名，永久签名，算法如有更改，需要重新注册用户
    static QString SignForPasswd(QString passwd){
        QByteArray md5 = QCryptographicHash::hash(passwd.toLocal8Bit(), QCryptographicHash::Md5);
        QString strMd5 = QString(md5.toHex()) + passwd;
        return QCryptographicHash::hash(passwd.toLocal8Bit(), QCryptographicHash::Md5).toHex().toUpper();
    };
    //通信key发放，传入签名源凭证，签名时间戳，通常使用登录接口的passwd字段,临时签名，算法可自由修改
    static QString SignForKey(QString src, qint64 timestamp){
        QString signStr = QString("%1+%2").arg(src).arg(timestamp);
        return QCryptographicHash::hash(signStr.toLocal8Bit(),QCryptographicHash::Md5).toHex().toUpper();
    };

};

#endif // SIGNTOOL_H
