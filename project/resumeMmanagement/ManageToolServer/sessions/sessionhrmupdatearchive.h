#ifndef SESSIONHRMUPDATEARCHIVE_H
#define SESSIONHRMUPDATEARCHIVE_H

#include "ReplyAndService/abssession.h"
#include <QObject>

class SessionHrmUpdateArchive : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionHrmUpdateArchive(AbsMsgHandle *handle, QObject *parent);

    // AbsSession interface
protected:
    void sessionRun();
};

#endif // SESSIONHRMUPDATEARCHIVE_H
