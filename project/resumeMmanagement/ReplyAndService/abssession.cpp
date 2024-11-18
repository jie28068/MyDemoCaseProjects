#include "abssession.h"
#include <QDateTime>

AbsSession::AbsSession(AbsMsgHandle* handle, QObject *parent) :
    QObject(parent),
    msgHandle(handle)
{

}

//void AbsSession::recvMsg(QByteArray& msg)
//{
//    Q_UNUSED(msg);
//}

void AbsSession::sessionStart(QVariantMap& sessionData)
{
    srcData = sessionData;
    responseParams.clear();
    msgParse();
    sessionRun();
    sessionResponse();
}

void AbsSession::sessionResponse()
{
    msgHandle->sendMsg(dataCache);
}

void AbsSession::msgParse()
{
    srcParams = srcData["Params"].toMap();
    mapcpy(srcInfo, srcData, "Key");
    mapcpy(srcInfo, srcData, "ServiceGroup");
    mapcpy(srcInfo, srcData, "Service");
    mapcpy(srcInfo, srcData, "Timestamp");
}

/**
 * @brief AbsSession::createResponse 创建回应
 * @param success   成功回应或失败回应
 * @param result    成功提示或失败原因
 */
void AbsSession::createResponse(bool success, QString result)
{
    QVariantMap responseMap;
    responseMap["Success"] = success;
    responseMap["Result"] = result;
    responseMap["Request"] = srcInfo;//来自会话初始信息
    responseMap["Params"] = responseParams;//来自会话过程创建的参数map
    responseMap["Timestamp"] = QDateTime::currentMSecsSinceEpoch();//当前时间戳
    QJsonObject jObject = QJsonObject::fromVariantMap(responseMap);
    QJsonDocument jDoc(jObject);
    dataCache = jDoc.toJson();//组织好回应的数据
}

void AbsSession::mapcpy(QVariantMap &tar, QVariantMap &src, QString key)
{
    tar.insert(key, src[key]);
}

