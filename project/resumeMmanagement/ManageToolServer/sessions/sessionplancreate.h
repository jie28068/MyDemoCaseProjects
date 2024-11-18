#ifndef SESSIONPLANCREATE_H
#define SESSIONPLANCREATE_H

#include "ReplyAndService/abssession.h"
#include <QObject>
#include "dbmanager.h"

class SessionPlanCreate : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionPlanCreate(AbsMsgHandle* handle, QObject *parent = nullptr);

    // AbsSession interface
protected:
    void sessionRun() override;
};

#endif // SESSIONPLANCREATE_H
