#include "sessionhrmupdatearchive.h"
#include "dbmanager.h"

SessionHrmUpdateArchive::SessionHrmUpdateArchive(AbsMsgHandle *handle, QObject *parent) : AbsSession(handle, parent)
{

}

void SessionHrmUpdateArchive::sessionRun()
{
//    QString sign = srcParams["sign"].toString();
//    if(sign.isEmpty())
//    {
//        createResponse(false, "档案更新失败,签名为空");
//        return;
//    }
    qDebug()<<"SessionHrmUpdateArchive::sessionRun()";

    int id = DBManager::instance().queryUserID(QString("name = '%1' ").arg(srcParams["src"].toString()));
    if(id==0){//不存在就注册
            QVariantMap params;
            params["name"] = srcParams["src"];
            params["type"] = srcParams["srctype"];
            params["email"] = QDateTime::currentMSecsSinceEpoch();
            params["tel_no"] = QDateTime::currentMSecsSinceEpoch();
            params["passwd"] = QDateTime::currentMSecsSinceEpoch();
            params["department"] = "无";
            if(DBManager::instance().newUserInfo(params)){
                id = DBManager::instance().queryUserID(QString("name='%1'").arg(srcParams["src"].toString()));

            }
    }
    srcParams["src"]=id;
    srcParams.remove("srctype");
    srcParams.remove("oldsrcid");
    srcParams.remove("levelname");
    srcParams.remove("age");
    qDebug()<<srcParams["level"];
    DBManager::instance().updateArchiveInfo(srcParams);
    createResponse(true, "档案更新成功");
    DBManager::instance().insertOneWorkLog(QString::number(msgHandle->userID()),QString("更新了<%1>的档案").arg(srcParams["name"].toString()));
}
