#ifndef SESSIONBASEUSERGROUPUPDATE_H
#define SESSIONBASEUSERGROUPUPDATE_H

#include "ReplyAndService/abssession.h"
#include <QObject>
#include "dbmanager.h"

class SessionBaseUserGroupUpdate : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionBaseUserGroupUpdate(AbsMsgHandle* handle, QObject *parent = nullptr);

    // AbsSession interface
protected:
    void sessionRun() override;
};

#endif // SESSIONBASEUSERGROUPUPDATE_H
