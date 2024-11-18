#include "sessionplanupdate.h"

SessionPlanUpdate::SessionPlanUpdate(AbsMsgHandle *handle, QObject *parent) : AbsSession(handle, parent)
{

}

void SessionPlanUpdate::sessionRun()
{
    QString new_note = srcParams["new_note"].toString();
    srcParams["new_note"] = QString("%1：%2").arg(msgHandle->name(), new_note);

   if(DBManager::instance().UpdateSession(srcParams))
   {
        createResponse(true, "日志更新成功");
        DBManager::instance().insertOneWorkLog(QString::number(msgHandle->userID()),"更新了计划日志");
   }
   else
   {
        createResponse(false, "日志更新失败,重复提交");
   }

}
