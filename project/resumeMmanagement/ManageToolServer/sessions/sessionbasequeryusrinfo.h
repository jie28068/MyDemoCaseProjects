#ifndef SESSIONBASEQUERYUSRINFO_H
#define SESSIONBASEQUERYUSRINFO_H


#include "ReplyAndService/abssession.h"
#include <QObject>
#include "dbmanager.h"

class SessionBaseQueryUsrInfo : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionBaseQueryUsrInfo(AbsMsgHandle* handle, QObject *parent = nullptr);
    // AbsSession interface
protected:
    void sessionRun() override;
};

#endif // SESSIONBASEQUERYUSRINFO_H
