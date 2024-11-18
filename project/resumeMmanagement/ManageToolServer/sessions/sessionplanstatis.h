#ifndef SESSIONPLANSTATIS_H
#define SESSIONPLANSTATIS_H

#include "ReplyAndService/abssession.h"
#include <QObject>
#include "dbmanager.h"

class SessionPlanStatis : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionPlanStatis(AbsMsgHandle* handle, QObject *parent = nullptr);
    // AbsSession interface
protected:
    void sessionRun() override;
};

#endif // SESSIONPLANSTATIS_H
