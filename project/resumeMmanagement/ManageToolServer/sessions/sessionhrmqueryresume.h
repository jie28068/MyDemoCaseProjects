#ifndef SESSIONHRMQUERYRESUME_H
#define SESSIONHRMQUERYRESUME_H

#include "ReplyAndService/abssession.h"
#include <QObject>

class SessionHrmQueryResume : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionHrmQueryResume(AbsMsgHandle *handle, QObject *parent);

    // AbsSession interface
protected:
    void sessionRun();

private:
    QString archivePath;

private slots:
    void archiveTransmit();
};

#endif // SESSIONHRMQUERYRESUME_H
