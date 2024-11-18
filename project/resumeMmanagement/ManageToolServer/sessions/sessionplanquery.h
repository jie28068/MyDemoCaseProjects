#ifndef SESSIONPLANQUERY_H
#define SESSIONPLANQUERY_H

#include "ReplyAndService/abssession.h"
#include <QObject>
#include "dbmanager.h"

class SessionPlanQuery : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionPlanQuery(AbsMsgHandle* handle, QObject *parent = nullptr);
    // AbsSession interface
protected:
    void sessionRun() override;
};

#endif // SESSIONPLANQUERY_H
