#include "serviceobject.h"
#include <QMetaObject>
#include <QDebug>
#include <QMetaObject>

ServiceObject::ServiceObject(QObject* handle)
{
    objHandle = handle;
}

/**
 * @brief ServiceWidget::serviceResponse    解析服务组和服务名，反射到对应的接口处理方法
 * 例：
 * 服务组：Base
 * 服务：Regist
 * 应定义一个对应的回调函数：
 * Q_INVOKABLE void BaseRegist(QVariantMap responseData);
 * 此类方法都需要用宏Q_INVOKABLE修饰，才能完成反射
 * @param responseData
 */
void ServiceObject::serviceResponse(QVariantMap responseData)
{
    QVariantMap request = responseData["Request"].toMap();
    QString serviceGroup = request["ServiceGroup"].toString();
    QString service = request["Service"].toString();
    QString servicePath = QString("%1%2").arg(serviceGroup, service);
    QByteArray invokeFunction = servicePath.toUtf8();

    objHandle->metaObject()->invokeMethod(objHandle, invokeFunction.data(), Q_ARG(QVariantMap, responseData));
}
