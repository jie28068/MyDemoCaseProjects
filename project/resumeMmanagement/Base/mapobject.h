#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include <QObject>
#include <QVariantMap>
#include <QMetaProperty>
#include <QtDebug>

class MapObject
{
public:
    explicit MapObject(QObject* obj);
    void updateSrcData(QVariantMap& srcMap){
        QVariantMap::iterator it;
        for(it = srcMap.begin(); it!=srcMap.end(); it++)
        {
            QByteArray key = it.key().toLocal8Bit();

            if(mapObj->dynamicPropertyNames().contains(key))//只修改已存在的属性
                mapObj->setProperty(key.data(), it.value());
        }
    }
    QVariantMap getDataMap(){
        int i=0;
        QVariantMap dataMap;
        for(i=0; i<mapObj->metaObject()->propertyCount(); i++)
        {
            QMetaProperty oneProperty = mapObj->metaObject()->property(i);
            if(mapObj->property(oneProperty.name()) == "objectName")
                continue;
            qDebug()<<oneProperty.name()<<mapObj->property(oneProperty.name());
            dataMap.insert(oneProperty.name(), mapObj->property(oneProperty.name()));
        }
        return dataMap;
    }

private:
    QObject* mapObj;
};

#endif // MAPOBJECT_H
