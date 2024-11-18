#include "sessionplancreate.h"
#include <QDateTime>
#include "Base/FormatTranslator.h"

SessionPlanCreate::SessionPlanCreate(AbsMsgHandle* handle, QObject *parent) : AbsSession(handle, parent)
{

}

void SessionPlanCreate::sessionRun()
{
    //补全参数
    srcParams["create_time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    srcParams["role_create"] = msgHandle->userID();

    int gID = srcParams["role_group"].toInt();
    QVariantList idList = DBManager::instance().queryGroupUserID(gID);
    int targetID = srcParams["role_target"].toInt();
    if(targetID)
        idList<<targetID;
    qDebug()<<"idlist"<<idList;
    srcParams["target_list"] = idList;

    QString msg = QString(R"({"topic":"新计划通知","msg":"你有新的计划待完成"})");
    msgHandle->sendMsgToUsers(idList, msg.toUtf8());

    int successCount = DBManager::instance().SessionPlanCreate(srcParams);
    if(successCount > 0)
    {
        createResponse(true, QString("创建成功了%1个计划").arg(successCount));
        DBManager::instance().insertOneWorkLog(QString::number(msgHandle->userID()),QString("创建成功了%1个计划").arg(successCount));
    }
    else
    {
        createResponse(false, "计划创建失败");
    }
}
