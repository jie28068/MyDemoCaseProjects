#include "objectfactory.h"

QMap<QString, const QMetaObject*> ObjectFactory::metaMap;
ObjectFactory::ObjectFactory(QObject *parent) : QObject(parent)
{

}

/**
 * @brief ObjectFactory::registeObject 注册一个QObject子类
 * 例：ObjectFactory::registeObject(&SessionBaseLogin::staticMetaObject);
 * @param metaObj
 */
void ObjectFactory::registeObject(const QMetaObject *metaObj)
{
    metaMap.insert(metaObj->className(), metaObj);
}
