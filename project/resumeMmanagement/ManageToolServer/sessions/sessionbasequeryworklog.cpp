#include "sessionbasequeryworklog.h"

SessionBaseQueryWorkLog::SessionBaseQueryWorkLog(AbsMsgHandle *handle, QObject *parent) : AbsSession(handle, parent)
{

}

void SessionBaseQueryWorkLog::sessionRun()
{
    qDebug()<<"[SessionBaseQueryWorkLog::sessionRun]";
    //srcParams中的数据为过滤条件

    QVariantList maplist=DBManager::instance().SessionQueryWorkLog(srcParams);
    if(maplist.count()!=0)
    {
        responseParams.insert("work_log",maplist);        
    }
    responseParams.insert("user_id",srcParams["userId"].toString());
    //不管查寻到与否 都查询成功
    createResponse(true, "工作日志查询成功");
}
