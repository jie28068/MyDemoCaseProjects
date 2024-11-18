#include "sessionplanstatis.h"

SessionPlanStatis::SessionPlanStatis(AbsMsgHandle *handle, QObject *parent) : AbsSession(handle, parent)
{

}

void SessionPlanStatis::sessionRun()
{
    qDebug()<<"[SessionPlanStatis::sessionRun]"<<srcData;
    //srcParams中的数据为过滤条件

    QVariantList maplist=DBManager::instance().SessionPlanStatis(srcParams);
    if(maplist.count()!=0)
    {
        responseParams.insert("plan_statis",maplist);
    }
    //不管查寻到与否 都查询成功
    createResponse(true, "计划总览查询会话成功");
}
