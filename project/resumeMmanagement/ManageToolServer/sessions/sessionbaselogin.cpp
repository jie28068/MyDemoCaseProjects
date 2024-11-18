#include "sessionbaselogin.h"
#include <QtDebug>
#include "dbmanager.h"

SessionBaseLogin::SessionBaseLogin(AbsMsgHandle *handle, QObject *parent):
    AbsSession(handle, parent)
{

}

/**
 * @brief SessionBaseLogin::sessionRun
 * 登录接口逻辑：验证密码，返回登录结果
 * session的最简实现流程就是在sessionRun中使用createResponse生成返回数据，
 * sessionStart会自动完成数据解析，数据回应过程
 */
void SessionBaseLogin::sessionRun()
{
    qDebug()<<"[SessionBaseLogin::sessionRun]"<<srcData;
    int LoginResult;
    QVariantMap map=DBManager::instance().BaseLoginHandle(srcParams,LoginResult);           //用户登录会话处理
    if(map.count()==0)   //返回数据是空的
    {
        if(LoginResult==NoUserMessage)
        {
           createResponse(false, "登录失败,无用户信息");
        }
        if(LoginResult==PasswordError)
        {
           createResponse(false, "登录失败,密码错误");
        }
    }
    else
    {
      responseParams=map;     //设置返回数据
      msgHandle->setUserID(map["id"].toInt());
      msgHandle->setName(map["name"].toString());
      createResponse(true, "登录成功");
      DBManager::instance().insertOneWorkLog(QString::number(msgHandle->userID()),QString("登录系统"));
    }
}
