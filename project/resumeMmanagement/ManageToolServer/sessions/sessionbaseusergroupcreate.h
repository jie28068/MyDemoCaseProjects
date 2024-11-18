#ifndef SESSIONBASEUSERGROUPCREATE_H
#define SESSIONBASEUSERGROUPCREATE_H

#include "ReplyAndService/abssession.h"
#include <QObject>
#include "dbmanager.h"

class SessionBaseUserGroupCreate : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionBaseUserGroupCreate(AbsMsgHandle* handle, QObject *parent = nullptr);

    // AbsSession interface
protected:
    void sessionRun() override;
};

#endif // SESSIONBASEUSERGROUPCREATE_H
