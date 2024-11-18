#ifndef SESSIONFACTORY_H
#define SESSIONFACTORY_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QtDebug>
#include <QMap>
#include <qlogging.h>
#include "Base/objectfactory.h"
#include "abssession.h"
#include "ServiceDefines.h"
#include "absmsghandle.h"

class SessionFactory : public QObject
{
    Q_OBJECT
public:
    explicit SessionFactory(QObject *parent = nullptr);
    static void registeSessions();
    static void registeSession(const QMetaObject &metaObj);
    static QObject* createSession(QString serviceGroup, QString service, AbsMsgHandle* handle){
        QString session = QString("Session%1%2").arg(serviceGroup).arg(service).toLocal8Bit();
        const QMetaObject* metaObj = metaMap.value(session, NULL);
//        qDebug()<<"metamap"<<metaMap<<metaObj;

        if(metaObj == NULL){
            qDebug()<<QString("创建%1失败,请在SessionFactory::registeSessions进行注册").arg(session);
//            Q_ASSERT(metaObj != NULL);
            return NULL;
        }
        QObject *obj = metaObj->newInstance(Q_ARG(AbsMsgHandle*, handle), Q_ARG(QObject*, nullptr));
        return obj;
    };

signals:

private:
    static QMap<QString, const QMetaObject*> metaMap;
};

#endif // SESSIONFACTORY_H
