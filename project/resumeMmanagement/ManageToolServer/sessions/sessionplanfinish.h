#ifndef SESSIONPLANFINISH_H
#define SESSIONPLANFINISH_H

#include "ReplyAndService/abssession.h"
#include <QObject>
#include "dbmanager.h"

class SessionPlanFinish : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionPlanFinish(AbsMsgHandle* handle, QObject *parent = nullptr);

    // AbsSession interface
protected:
    void sessionRun() override;
};

#endif // SESSIONPLANFINISH_H
