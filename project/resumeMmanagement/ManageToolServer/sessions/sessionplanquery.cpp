#include "sessionplanquery.h"

SessionPlanQuery::SessionPlanQuery(AbsMsgHandle *handle, QObject *parent) : AbsSession(handle, parent)
{

}

void SessionPlanQuery::sessionRun()
{
//    qDebug()<<"[SessionPlanQuery::sessionRun]"<<srcData;
    //srcParams中的数据为过滤条件
    srcParams["id"] = msgHandle->userID();

    QVariantList maplist=DBManager::instance().SessionPlanQuery(srcParams);
    if(maplist.count()!=0)
    {
        responseParams.insert("plan_list",maplist);
    }
    //不管查寻到与否 都查询成功
    createResponse(true, "计划查询会话成功");
}
