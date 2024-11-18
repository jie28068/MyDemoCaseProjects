#ifndef MTREPLY_H
#define MTREPLY_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QJsonObject>
#include <QJsonDocument>
#include "serviceobject.h"

class MTReply : public QObject
{
    Q_OBJECT
public:
    explicit MTReply(QString optKey, QString groupName, QString serviceName, ServiceObject *handle = nullptr);
    void setParams(const QVariantMap& paramsMap);//写入参数params
    void updateTimestamp();//刷新时间戳
    qint64 getTimestamp();
    QJsonObject getReplyJson();//序列化为json
    QByteArray getReplyBytes();//序列化为byte
    QVariantMap& responseData();
    void recvResponseData(const QVariantMap& responseData);

signals:
    void responseReady();

private:
    QString key;//操作人唯一标识
    QString serviceGroup;//接口组
    QString service;//接口
    qint64 timestamp;//时间戳
    QVariantMap params;//附带参数
    QVariantMap response;
    ServiceObject* serviceHandle;
};

#endif // MTREPLY_H
