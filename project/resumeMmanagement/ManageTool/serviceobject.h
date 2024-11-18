#ifndef SERVICEOBJECT_H
#define SERVICEOBJECT_H

#include <QObject>
#include <QVariantMap>

class ServiceObject
{
public:
    explicit ServiceObject(QObject* handle);

public slots:
    virtual void serviceResponse(QVariantMap responseData);

private:
    QObject* objHandle;

signals:
//    void return_mainPage();

//    void serviceReply(QString serviceGroup, QString service, QVariantMap& params);

};

#endif // SERVICEOBJECT_H
