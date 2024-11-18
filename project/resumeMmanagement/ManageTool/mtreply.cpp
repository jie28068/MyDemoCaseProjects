#include "mtreply.h"
#include <QDateTime>

MTReply::MTReply(QString optKey, QString groupName, QString serviceName, ServiceObject *handle) :
    QObject(nullptr),
    key(optKey),
    serviceGroup(groupName),
    service(serviceName),
    timestamp(QDateTime::currentMSecsSinceEpoch()),
    serviceHandle(handle)
{

}

void MTReply::setParams(const QVariantMap &paramsMap)
{
    params = paramsMap;
}

void MTReply::updateTimestamp()
{
    timestamp = QDateTime::currentMSecsSinceEpoch();
}

qint64 MTReply::getTimestamp()
{
    return timestamp;
}

QJsonObject MTReply::getReplyJson()
{
    QVariantMap paramsMap;
    paramsMap["Key"] = key;
    paramsMap["ServiceGroup"] = serviceGroup;
    paramsMap["Service"] = service;
    paramsMap["Params"] = params;
    paramsMap["Timestamp"] = timestamp;

    return QJsonObject::fromVariantMap(paramsMap);
}

QByteArray MTReply::getReplyBytes()
{
    QJsonObject jObj = getReplyJson();
    QJsonDocument jDoc(jObj);
    return jDoc.toJson();
}

QVariantMap &MTReply::responseData()
{
    return response;
}

void MTReply::recvResponseData(const QVariantMap &responseData)
{
    serviceHandle->serviceResponse(responseData);
}
