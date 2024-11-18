#include "sessionbaseusergroupcreate.h"

SessionBaseUserGroupCreate::SessionBaseUserGroupCreate(AbsMsgHandle *handle, QObject *parent) : AbsSession(handle, parent)
{

}

void SessionBaseUserGroupCreate::sessionRun()
{
    bool success = DBManager::instance().newUserGroup(srcParams);
    if(success)
    {
        createResponse(true, "用户组创建成功");
        DBManager::instance().insertOneWorkLog(QString::number(msgHandle->userID()),"创建用户组");
    }
    else
    {
        createResponse(false, "用户组创建失败");
    }
}
