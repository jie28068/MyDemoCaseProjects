#ifndef ABSMTMODEL_H
#define ABSMTMODEL_H
#include <QVariantMap>
#include <QObject>
#include <QMetaProperty>
#include <QtDebug>
#include <QAbstractTableModel>

class AbsMTModel: public QObject
{
    Q_OBJECT
public:
    AbsMTModel();
    virtual void updateSrcData(QVariantMap& srcMap){
        QVariantMap::iterator it;
        for(it = srcMap.begin(); it!=srcMap.end(); it++)
        {
            QByteArray key = it.key().toLocal8Bit();
            setProperty(key.data(), it.value());
        }
    }
    virtual QVariantMap getDataMap(){
        int i=0;
        QVariantMap dataMap;
        for(i=0; i<metaObject()->propertyCount(); i++)
        {
            QMetaProperty oneProperty = metaObject()->property(i);
            if(property(oneProperty.name()) == "objectName")
                continue;
            qDebug()<<oneProperty.name()<<property(oneProperty.name());
            dataMap.insert(oneProperty.name(), property(oneProperty.name()));
        }
        return dataMap;
    }
protected:

};

#endif // ABSMTMODEL_H
