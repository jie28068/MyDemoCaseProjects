#include "sessionbasequerygroupusers.h"

SessionBaseQueryGroupUsers::SessionBaseQueryGroupUsers(AbsMsgHandle* handle, QObject *parent) : AbsSession(handle, parent)
{

}

/**
 * @brief SessionBaseQueryGroupUsers::sessionRun    查询指定组的所有用户
 */
void SessionBaseQueryGroupUsers::sessionRun()
{
    int id = srcParams["id"].toInt();

    QVariantList user_list = DBManager::instance().queryGroupUsers(id);
    responseParams["user_list"] = user_list;
    responseParams["group_id"] = id;
    createResponse(true, "");//失败查询结果为空，所以暂无返回提示信息
}
