#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QtDebug>
#include <QMap>
#include <qlogging.h>

class ObjectFactory : public QObject
{
    Q_OBJECT
public:
    explicit ObjectFactory(QObject *parent = nullptr);
    static void registeObject(const QMetaObject* metaObj);

    /**
     * @brief createObject 根据类名创建实例，有较大限制，最好先不用
     * @param className
     * @return
     */
    static QObject* createObject(QString className){
        const QMetaObject* metaObj = metaMap.value(className, NULL);

        qDebug()<<"Create obj"<<className;
        if(metaObj == NULL){

            qFatal("Obj 创建失败");
            return NULL;
        }
        QObject *obj = metaObj->newInstance(Q_ARG(QObject*, nullptr));
        return obj;
    };

signals:

private:
    static QMap<QString, const QMetaObject*> metaMap;
};

#endif // OBJECTFACTORY_H
