#ifndef ABSSESSION_H
#define ABSSESSION_H
#include <QObject>
#include <QVariantMap>
#include <QJsonObject>
#include <QJsonDocument>
#include "absmsghandle.h"
#include <QDateTime>

class AbsMsgHandle;

class AbsSession : public QObject
{
    Q_OBJECT
public:
    explicit AbsSession(AbsMsgHandle* handle, QObject *parent = nullptr);
    //会话开始，有会话数据时，此方法就会被调用，如果会话过程是异步的，请重写此方法。
    virtual void sessionStart(QVariantMap &sessionData);

signals:

//private slots:
//    virtual void recvMsg(QByteArray& msg);

protected:
    //业务过程相关：包含需要的数据库相关操作、业务相关计算、业务过程中的必要响应，以及准备好响应数据(调用createResponse方法)
    virtual void sessionRun()=0;
    //业务相关响应
    virtual void sessionResponse();
    //解析初始数据
    virtual void msgParse();
    //创建回应数据
    virtual void createResponse(bool success, QString result);

    //在2个map间拷贝键为key的数据
    void mapcpy(QVariantMap &tar, QVariantMap &src, QString key);

protected:
    AbsMsgHandle* msgHandle;
    QByteArray dataCache;//缓存待发送数据
    QVariantMap srcParams;//初始会话的参数信息[接口参数表]
    QVariantMap srcInfo;//会话的公共信息[操作人、接口组、接口、时间戳]
    QVariantMap srcData;//会话的初始全部信息
    QVariantMap responseParams;//回应参数表

};

#endif // ABSSESSION_H
