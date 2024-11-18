#ifndef SESSIONBASELOGIN_H
#define SESSIONBASELOGIN_H

#include "ReplyAndService/abssession.h"
#include <QObject>

#include "dbmanager.h"
class SessionBaseLogin : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionBaseLogin(AbsMsgHandle* handle, QObject *parent = nullptr);
    void sessionRun();
    // AbsSession interface
private slots:
//    void recvMsg(QByteArray msg);

private:
//    void sessionRun();

};

#endif // SESSIONBASELOGIN_H
