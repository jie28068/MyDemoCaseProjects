#ifndef SESSIONPLANNOTIFY_H
#define SESSIONPLANNOTIFY_H

#include "ReplyAndService/abssession.h"
#include <QObject>
#include "dbmanager.h"

class SessionPlanNotify : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionPlanNotify(AbsMsgHandle* handle, QObject *parent = nullptr);

protected:
    void sessionRun() override;

};

#endif // SESSIONPLANNOTIFY_H
