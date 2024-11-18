#include "sessionplanfinish.h"
#include <QDateTime>

SessionPlanFinish::SessionPlanFinish(AbsMsgHandle *handle, QObject *parent) : AbsSession(handle, parent)
{

}

void SessionPlanFinish::sessionRun()
{
    srcParams["role_complete"] = msgHandle->userID();
    srcParams["finish_time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    if(DBManager::instance().SessionPlanFinish(srcParams))
    {
         createResponse(true, "完成计划成功");
         DBManager::instance().insertOneWorkLog(QString::number(msgHandle->userID()),QString("完成了计划ID:%1").arg(srcParams["id"].toString()));
    }
    else
    {
         createResponse(false, "完成计划失败");
    }
}
