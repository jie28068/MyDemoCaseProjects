#ifndef SESSIONBASEQUERYWORKLOG_H
#define SESSIONBASEQUERYWORKLOG_H


#include "ReplyAndService/abssession.h"
#include <QObject>
#include "dbmanager.h"

class SessionBaseQueryWorkLog : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionBaseQueryWorkLog(AbsMsgHandle* handle, QObject *parent = nullptr);
    // AbsSession interface
protected:
    void sessionRun() override;
};

#endif // SESSIONBASEQUERYWORKLOG_H
