#ifndef SESSIONHRMUPLOADRESUME_H
#define SESSIONHRMUPLOADRESUME_H

#include "ReplyAndService/abssession.h"
#include <QObject>
#include <iostream>
#include <Base/PathTool.h>

float TextCompare(std::string &ref1, std::string &ref2);
class SessionHrmUploadResume : public AbsSession
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SessionHrmUploadResume(AbsMsgHandle* handle, QObject *parent = nullptr);
     void contrastResume();
     bool compareName(QString name,QString archivename);
     bool compareBirthday(QString birthday,QString archivebirthday);
     bool compareExperience(QStringList experience,QStringList archiveexperience);
     float textCompare(QString &UploadText, QString &ArchiveText);
    // AbsSession interface
protected:
    void sessionRun();
    QString UploadResumeformat;
    QString UploadResumesign;
    QString Uploadbirthday;
    QStringList Uploadexperience;
private slots:
    void streamMsg(bool success, QString msg);
    void resumeCompareTest();
};

#endif // SESSIONHRMUPLOADRESUME_H
