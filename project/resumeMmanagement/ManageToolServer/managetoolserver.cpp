#include "managetoolserver.h"
#define MT_SERVER_PORT 10086
#include "ReplyAndService/sessionfactory.h"
#include "Model/usermodel.h"
#include "sessions.h"

ManageToolServer::ManageToolServer(QObject *parent) : QObject(parent)
{
    qInfo()<<"ManageToolServer start";
    socketServer = nullptr;
    registSessions();
    initSocketServer();
}

void ManageToolServer::initSocketServer()
{
    if(socketServer){
        qWarning("The service does not need to be initialized again.");
        return;
    }
    socketServer = new QTcpServer(this);
    socketServer->listen(QHostAddress::Any, MT_SERVER_PORT);
    qDebug()<<"SocketServer listening in port:"<<MT_SERVER_PORT;
    connect(socketServer, &QTcpServer::newConnection, this, &ManageToolServer::newSocketConnect);
}

void ManageToolServer::registSessions()
{
    SessionFactory::registeSession(SessionBaseLogin::staticMetaObject);
    SessionFactory::registeSession(SessionBaseRegist::staticMetaObject);
    SessionFactory::registeSession(SessionHrmFilterResume::staticMetaObject);
    SessionFactory::registeSession(SessionHrmQueryResume::staticMetaObject);
    SessionFactory::registeSession(SessionHrmUpdateArchive::staticMetaObject);
    SessionFactory::registeSession(SessionHrmUploadArchive::staticMetaObject);
    SessionFactory::registeSession(SessionHrmUploadResume::staticMetaObject);

    SessionFactory::registeSession(SessionBaseUserGroupCreate::staticMetaObject);
    SessionFactory::registeSession(SessionBaseUserGroupUpdate::staticMetaObject);
    SessionFactory::registeSession(SessionPlanNotify::staticMetaObject);
    SessionFactory::registeSession(SessionPlanCreate::staticMetaObject);
    SessionFactory::registeSession(SessionPlanUpdate::staticMetaObject);
    SessionFactory::registeSession(SessionPlanFinish::staticMetaObject);
    SessionFactory::registeSession(SessionPlanQuery::staticMetaObject);
    SessionFactory::registeSession(SessionBaseQueryGroupInfo::staticMetaObject);
    SessionFactory::registeSession(SessionBaseQueryGroupUsers::staticMetaObject);
    SessionFactory::registeSession(SessionPlanStatis::staticMetaObject);
    SessionFactory::registeSession(SessionBaseQueryWorkLog::staticMetaObject);
    SessionFactory::registeSession(SessionBaseQueryUsrInfo::staticMetaObject);
}

void ManageToolServer::newSocketConnect()
{
    if(socketServer->hasPendingConnections()){
        QTcpSocket* newSkt = socketServer->nextPendingConnection();
        qDebug()<<"[ManageToolServer] new connect:"<<newSkt->peerAddress().toString();
        AbsMsgHandle* msgHandle = new SocketHandle(newSkt);        
        connect(msgHandle, &SocketHandle::destroyRequire, this, &ManageToolServer::deleteSocketConnect);
        connect(msgHandle, &AbsMsgHandle::userIdChanged, this, &ManageToolServer::regConnectID);
        connect(msgHandle, &SocketHandle::msgToUser, this, &ManageToolServer::sendMsg);
    }
}

void ManageToolServer::deleteSocketConnect()
{
    SocketHandle* msgHandle = qobject_cast<SocketHandle*>(sender());
    qDebug()<<"[ManageToolServer] delete connect:"<<msgHandle->socket()->peerAddress().toString();
    msgHandle->deleteLater();
}

void ManageToolServer::regConnectID()
{
    AbsMsgHandle* handle = qobject_cast<AbsMsgHandle*>(sender());
    qDebug()<<"regConnectID"<<handle->userID();
    msgPool.insert(handle->userID(), handle);
}

/**
 * @brief ManageToolServer::sendMsg 发送msg至指定id的客户端，目前无送达反馈
 * @param id    客户端用户id
 * @param msg   待发送msg
 */
void ManageToolServer::sendMsg(int id, QByteArray msg)
{
    AbsMsgHandle* handle = msgPool.value(id, nullptr);
    if(handle == nullptr)
        return;

    qDebug()<<"[ManageToolServer::sendMsg]";
    handle->sendMsg(msg);
}
