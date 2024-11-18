#ifndef SESSIONBASEREGIST_H
#define SESSIONBASEREGIST_H

#include "ReplyAndService/abssession.h"
#include <QObject>

class SessionBaseRegist : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionBaseRegist(AbsMsgHandle* handle, QObject *parent = nullptr);

    // AbsSession interface
protected:
    void sessionRun();
};

#endif // SESSIONBASEREGIST_H
