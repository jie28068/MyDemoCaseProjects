#include "sessionhrmqueryresume.h"
#include "dbmanager.h"
#include "Base/PathTool.h"
#include <QTimer>

SessionHrmQueryResume::SessionHrmQueryResume(AbsMsgHandle *handle, QObject *parent) : AbsSession(handle, parent)
{

}

/**
 * @brief SessionHrmQueryResume::sessionRun
 * 参数
 * [sign]：档案签名
 * 返回：
 *  sign：档案签名
 *  size：档案大小
 */
void SessionHrmQueryResume::sessionRun()
{
    QString sign = srcParams["sign"].toString();
    if(sign.isEmpty()){
        createResponse(false, "请求档案签名为空");
        return;
    }

    QVariantMap archiveInfo = DBManager::instance().getArchiveInfo(sign);
    qDebug()<<archiveInfo;
    if((archiveInfo["size"].toULongLong() == 0) || archiveInfo["format"].toString().isEmpty())
    {
        createResponse(false, "未找到档案信息");
        return;
    }

    responseParams = archiveInfo;

    archivePath = PathTool::findArchiveFile(sign, PathTool::archivePath());
    createResponse(true, "查询简历详情成功");
    QTimer::singleShot(200, this, &SessionHrmQueryResume::archiveTransmit);
}

void SessionHrmQueryResume::archiveTransmit()
{
    QFile f(archivePath);
    if(!f.open(QFile::ReadOnly)){
        createResponse(false, "简历传输失败");
        sessionResponse();
        return;
    }
    QByteArray fileBytes = f.readAll();
    msgHandle->sendMsg(fileBytes);
}
