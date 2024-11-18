#ifndef SESSIONBASEQUERYGROUPUSERS_H
#define SESSIONBASEQUERYGROUPUSERS_H

#include "ReplyAndService/abssession.h"
#include <QObject>
#include "dbmanager.h"

class SessionBaseQueryGroupUsers : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionBaseQueryGroupUsers(AbsMsgHandle* handle, QObject *parent = nullptr);
protected:
    void sessionRun() override;
};

#endif // SESSIONBASEQUERYGROUPUSERS_H
