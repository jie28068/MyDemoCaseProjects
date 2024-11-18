#include "sessionbaseusergroupupdate.h"

SessionBaseUserGroupUpdate::SessionBaseUserGroupUpdate(AbsMsgHandle* handle, QObject *parent) : AbsSession(handle, parent)
{

}

void SessionBaseUserGroupUpdate::sessionRun()
{
    //用户输入的操作数q:1为插入，2为删除
   bool success = DBManager::instance().SessionBaseUserGroupUpdate(srcParams);

   if(success)
       createResponse(true, "用户组成员变更成功");
   else
       createResponse(false, "用户组成员变更失败");
}
