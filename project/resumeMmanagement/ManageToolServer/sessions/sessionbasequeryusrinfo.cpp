#include "sessionbasequeryusrinfo.h"


SessionBaseQueryUsrInfo::SessionBaseQueryUsrInfo(AbsMsgHandle *handle, QObject *parent) : AbsSession(handle, parent)
{

}
void SessionBaseQueryUsrInfo::sessionRun()
{
    qDebug()<<"[SessionBaseQueryUsrInfo::sessionRun]";
    //srcParams中的数据为过滤条件
    srcParams["id"] = msgHandle->userID();

    QVariantList maplist=DBManager::instance().SessionBaseQueryUsrInfo(srcParams);
    if(maplist.count()!=0)
    {
        responseParams.insert("info_list",maplist);
    }
    //不管查寻到与否 都查询成功
    createResponse(true, "用户信息查询会话成功");
}
