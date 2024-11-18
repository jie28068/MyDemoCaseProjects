#ifndef SESSIONHRMFILTERRESUME_H
#define SESSIONHRMFILTERRESUME_H

#include "ReplyAndService/abssession.h"
#include <QObject>

class SessionHrmFilterResume : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionHrmFilterResume(AbsMsgHandle *handle, QObject *parent);

    // AbsSession interface
protected:
    void sessionRun();
};

#endif // SESSIONHRMFILTERRESUME_H
