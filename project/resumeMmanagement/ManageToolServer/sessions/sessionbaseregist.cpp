#include "sessionbaseregist.h"
#include "dbmanager.h"

SessionBaseRegist::SessionBaseRegist(AbsMsgHandle *handle, QObject *parent) : AbsSession(handle, parent)
{

}

void SessionBaseRegist::sessionRun()
{
    if(DBManager::instance().newUserInfo(srcParams))//注册成功
    {
        int id = DBManager::instance().queryUserID(QString("tel_no='%1'").arg(srcParams["tel_no"].toString()));
        responseParams.insert("id",id);
        createResponse(true, "注册成功");
//        sessionResponse();

        msgHandle->setUserID(id);
        DBManager::instance().insertOneWorkLog(QString::number(msgHandle->userID()),QString("注册了新用户:%1").arg(id));
    }
    else
    {
        createResponse(false, "注册失败");
//        sessionResponse();
    }
}
