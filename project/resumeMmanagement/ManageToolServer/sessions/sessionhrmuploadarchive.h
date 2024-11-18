#ifndef SESSIONHRMUPLOADARCHIVE_H
#define SESSIONHRMUPLOADARCHIVE_H

#include "ReplyAndService/abssession.h"
#include <QObject>

class SessionHrmUploadArchive : public AbsSession
{
    Q_OBJECT
    void archiveFileConfirm(QString sign);
public:
    Q_INVOKABLE explicit SessionHrmUploadArchive(AbsMsgHandle *handle, QObject *parent);

    // AbsSession interface
protected:
    void sessionRun() override;


};

#endif // SESSIONHRMUPLOADARCHIVE_H
