#include "sessionbasequerygroupinfo.h"
#include "dbmanager.h"

SessionBaseQueryGroupInfo::SessionBaseQueryGroupInfo(AbsMsgHandle *handle, QObject *parent) : AbsSession(handle, parent)
{

}

/**
 * @brief SessionBaseQueryGroupInfo::sessionRun 查询所有用户组信息
 */
void SessionBaseQueryGroupInfo::sessionRun()
{
    responseParams["group_info"] = DBManager::instance().queryGroupInfo();
    createResponse(true,"");
}
