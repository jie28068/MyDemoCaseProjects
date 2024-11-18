#include "absmsghandle.h"
#include "ReplyAndService/sessionfactory.h"
#include "Base/SignTool.h"
#include "Base/PathTool.h"
#include <QtDebug>
#include "Base/jsonspliter.h"

//句柄超时，10s
#define TIMEOUT_HANDLE 10000

AbsMsgHandle::AbsMsgHandle(QObject *parent) : QObject(parent)
{
    m_apiMode = MODE_JSON;
    m_userID = 0;
    setKey(QString());
    //超时检查句柄合法性
    QTimer::singleShot(TIMEOUT_HANDLE, this, &AbsMsgHandle::handleCheck);
}

void AbsMsgHandle::recvMsg(QByteArray &msg)
{
    QList<QByteArray> msgList = JsonSpliter::splitJsonStream(msg);

    foreach(QByteArray jMsg, msgList)
    {
        QJsonDocument jDoc = QJsonDocument::fromJson(jMsg);
        qDebug()<<jMsg.size()<<jDoc.toBinaryData().size();

        QVariantMap msgMap = jDoc.object().toVariantMap();
        QString serviceGroup = msgMap["ServiceGroup"].toString();
        QString service = msgMap["Service"].toString();
        QString servicePath = QString("%1%2").arg(serviceGroup, service);
        qDebug().noquote()<<"[AbsMsgHandle::recvMsg]"<<jMsg;

        if(!mapSession.contains(servicePath)){//会话不存在
            if(userID() == 0 && servicePath != "BaseLogin" && servicePath != "BaseRegist"){
                return;
            }
            QObject* sessionObj = SessionFactory::createSession(serviceGroup, service, this);
            if(!sessionObj){
                qDebug()<<"[AbsMsgHandle::recvMsg] create session failed"<<serviceGroup<<service;
                continue;;
            }
            mapSession[servicePath] = qobject_cast<AbsSession*>(sessionObj);
        }
        mapSession[servicePath]->sessionStart(msgMap);
    }
}

void AbsMsgHandle::recvStream(QByteArray &bytes)
{
    streamCache.append(bytes);
    if(streamCache.size() > streamSize)//流数据接收错误
    {
        emit streamRst(false, "传输失败：数据长度不匹配");
        waitForMsg();
    }
    if(streamCache.size() == streamSize)//流数据接收完成
    {
        if(streamSign == SignTool::SignForStream(streamCache))//数据签名匹配
        {
            saveStream(QString("%1.%2").arg(streamSign, streamFormat), streamCache);
            streamCache.clear();
            waitForMsg();
            emit streamRst(true, "传输完成");
        }
        else
        {
            emit streamRst(false, "传输失败：签名错误");
            streamCache.clear();
            waitForMsg();
        }
    }

}

/**
 * @brief AbsMsgHandle::waitForStream 准备接收数据流
 * @param sign  数据流签名
 * @param size  数据流大小
 */
void AbsMsgHandle::waitForStream(QString sign, QString format, quint64 size)
{
    streamSign = sign;
    streamSize = size;
    streamFormat = format;
    setApiMode(MODE_STREAM);
}

void AbsMsgHandle::waitForMsg()
{
    setApiMode(MODE_JSON);
}

void AbsMsgHandle::sendMsgToUser(int id, const QByteArray& msg)
{
    qDebug()<<"sendMsgToUser"<<msg;
    emit msgToUser(id, msg);
}

void AbsMsgHandle::sendMsgToUsers(QVariantList ids, const QByteArray& msg)
{
    foreach(QVariant id, ids)
    {
        sendMsgToUser(id.toInt(), msg);
    }
}

const QString &AbsMsgHandle::key() const
{
    return m_key;
}

void AbsMsgHandle::setKey(const QString &newKey)
{
    m_key = newKey;
}

void AbsMsgHandle::handleCheck()
{
   // if(userID() == 0)//超时未分配key，销毁通信句柄
  //      emit destroyRequire();
}

bool AbsMsgHandle::saveStream(QString fileName, QByteArray &bytes)
{
    QString filepath = PathTool::cachePath()+"/"+fileName;
    qDebug()<<"saveStream"<<filepath;
    QFile f(filepath);
    if(!f.open(QFile::WriteOnly))
    {
        qDebug()<<"[AbsMsgHandle::saveStream] failed:"<<filepath;
        return false;
    }
    f.write(bytes);
    f.close();
    return true;
}

int AbsMsgHandle::userID() const
{
    return m_userID;
}

void AbsMsgHandle::setUserID(int newUserID)
{
    qDebug()<<"setUserID:"<<newUserID;
    m_userID = newUserID;
    emit userIdChanged();
}

const QString &AbsMsgHandle::name() const
{
    return m_name;
}

void AbsMsgHandle::setName(const QString &newName)
{
    m_name = newName;
}
