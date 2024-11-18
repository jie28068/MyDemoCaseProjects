#ifndef SESSIONBASEQUERYGROUPINFO_H
#define SESSIONBASEQUERYGROUPINFO_H

#include "ReplyAndService/abssession.h"
#include <QObject>

class SessionBaseQueryGroupInfo : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionBaseQueryGroupInfo(AbsMsgHandle *handle, QObject *parent);

    // AbsSession interface
protected:
    void sessionRun();
};

#endif // SESSIONBASEQUERYGROUPINFO_H
