#ifndef SESSIONPLANUPDATE_H
#define SESSIONPLANUPDATE_H

#include "ReplyAndService/abssession.h"
#include <QObject>
#include "dbmanager.h"
class SessionPlanUpdate : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionPlanUpdate(AbsMsgHandle* handle, QObject *parent = nullptr);

    // AbsSession interface
protected:
    void sessionRun() override;
};

#endif // SESSIONPLANUPDATE_H
