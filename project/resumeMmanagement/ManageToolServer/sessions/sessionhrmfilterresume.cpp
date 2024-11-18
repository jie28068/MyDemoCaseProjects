#include "sessionhrmfilterresume.h"
#include "dbmanager.h"

SessionHrmFilterResume::SessionHrmFilterResume(AbsMsgHandle *handle, QObject *parent) : AbsSession(handle, parent)
{

}

void SessionHrmFilterResume::sessionRun()
{
    qDebug()<<"[SessionHrmFilterResume::sessionRun]"<<srcData;
    //srcParams中的数据为过滤条件

    QVariantList maplist=DBManager::instance().HrmFilteResumeHandle(srcParams);    //查询简历列表会话处理
    if(maplist.count()!=0)
    {
        responseParams.insert("resume_list",maplist);
    }
    //不管查寻到与否 都查询成功
    createResponse(true, "查询简历列表成功");

}
