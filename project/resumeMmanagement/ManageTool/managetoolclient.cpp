#include "managetoolclient.h"
#include <QFile>

#include "ReplyAndService/ServiceDefines.h"
#include "Base/FormatTranslator.h"
#include "Base/SignTool.h"
#include "Base/PathTool.h"
#include "Base/settings.h"
#include "Base/jsonspliter.h"
#include <QTimer>

#define KL_SERVER_DEV "kl.mt.com"

ManageToolClient::ManageToolClient(QObject *parent) : QObject(parent)
{
    socketClient = nullptr;
    userKey = QString();
    m_apiMode = MODE_JSON;
    initSocket();
    qDebug()<<"connectToServer"<<Settings::getServer();
    connectToServer(Settings::getServer(),10086);
}

void ManageToolClient::setUserInfo(QVariantMap info)
{
//    foreach(QString key, info.keys())
//    {
//        QByteArray keyByte = key.toLocal8Bit();
//        userInfo.setProperty(keyByte.data(), info.value(key));
//    }
    userInfo.updateSrcData(info);
}

UserModel *ManageToolClient::getUserInfo()
{
    return &userInfo;
}

void ManageToolClient::initSocket()
{
    if(socketClient){
        qWarning("The client does not need to connect server again.");
        return;
    }
    socketClient = new QTcpSocket(this);
    connect(socketClient, &QTcpSocket::stateChanged, this, &ManageToolClient::stateChanged);
    connect(socketClient, &QTcpSocket::readyRead, this, &ManageToolClient::readMsg);
}

void ManageToolClient::connectToServer(QString serverAddr, quint16 serverPort)
{
    socketClient->connectToHost(serverAddr, serverPort);
    Settings::setServer(serverAddr);
}

void ManageToolClient::connectToServer(QHostAddress serverAddr, quint16 serverPort)
{
    socketClient->connectToHost(serverAddr, serverPort);
    Settings::setServer(serverAddr.toString());
}

void ManageToolClient::recvMsg(QByteArray &msg)
{
    QList<QByteArray> listJson = JsonSpliter::splitJsonStream(msg);

    foreach (QByteArray jMsg, listJson) {
        QVariantMap msgMap = FormatTransLator::ByteToMap(jMsg);
        QVariantMap requestMap = msgMap["Request"].toMap();
        qint64 timestamp = requestMap["Timestamp"].toLongLong();
        MTReply* reply = mapReply.value(timestamp, nullptr);
        if(reply == nullptr){
            recvNotify(msgMap);
            continue;
        }
        reply->recvResponseData(msgMap);
        //延迟释放
//        QTimer::singleShot(200,this,[this,timestamp](){
//            MTReply* reply = mapReply.value(timestamp, nullptr);
//            mapReply.remove(timestamp);
//            if(reply)
//                reply->deleteLater();
//        });
    }
}

void ManageToolClient::recvStream(QByteArray &bytes)
{
    streamCache.append(bytes);
    if(fCache)
        fCache->write(bytes);
    if(streamCache.size() > streamSize)//流数据接收错误
    {
        emit streamRst(false, "传输失败：数据长度不匹配");
        fCache->remove();
        fCache->deleteLater();
        fCache = nullptr;
        waitForMsg();
    }
    if(streamCache.size() == streamSize)//流数据接收完成
    {
        if(streamSign == SignTool::SignForStream(streamCache))//数据签名匹配
        {
            if(fCache)
            {
                emit streamRst(true, fCache->fileName());
                fCache->close();
                fCache->deleteLater();
                fCache = nullptr;
            }
            emit newArchiveStream(streamCache);
            saveStream(streamSign, streamCache);
            streamCache.clear();
            waitForMsg();
        }
        else
        {
            if(fCache)
            {
                fCache->remove();
                fCache->deleteLater();
                fCache = nullptr;
            }
            emit streamRst(false, "传输失败：签名错误");
            streamCache.clear();
        }
        waitForMsg();
    }
}

void ManageToolClient::recvNotify(QVariantMap &msgMap)
{
    QString topic = msgMap["topic"].toString();
    if(topic.isEmpty())
        return;

    QString msg = msgMap["msg"].toString();
    emit newNotify(topic, msg);
}

void ManageToolClient::waitForStream(QString sign,QString format, quint64 size)
{
    QString filePath = PathTool::clientCache() + QString("/%1.%2").arg(sign, format);
    fCache = new QFile(filePath);
    fCache->open(QFile::WriteOnly);
    streamFormat = format;
    streamSign = sign;
    streamSize = size;
    setApiMode(MODE_STREAM);
}

void ManageToolClient::waitForMsg()
{
    setApiMode(MODE_JSON);
}

bool ManageToolClient::saveStream(QString fileName, QByteArray &bytes)
{
    QString filepath = PathTool::cachePath()+"/"+fileName;
//    QString filepath = fileName;
    QFile f(filepath);
    if(!f.open(QFile::WriteOnly))
    {
        qDebug()<<"[ManageToolClient::saveStream] failed:"<<filepath;
        return false;
    }
    f.write(bytes);
    f.close();
    return true;
}

void ManageToolClient::stateChanged(QAbstractSocket::SocketState state)
{
    qDebug()<<"ManageToolClient::stateChanged:"<<state;
    if(state == QTcpSocket::UnconnectedState)
        connectToServer(Settings::getServer(),10086);
    else if(state == QTcpSocket::ConnectedState)
        emit connectRecover();
}

void ManageToolClient::readMsg()
{
    QByteArray msg = socketClient->readAll();
    qDebug()<<apiMode();
    qDebug().noquote()<<"[read msg]"<<msg;

    if(apiMode() == MODE_JSON)
        recvMsg(msg);
    else
        recvStream(msg);
}

void ManageToolClient::sendMsg(QByteArray &msg)
{
    if(apiMode() != MODE_JSON){//只有当JSON传输模式的时候才允许使用此方法传输
        qDebug()<<"[ManageToolClient::sendMsg]"<<"api mode error,current mode is"<<apiMode();
        return;
    }
    qDebug().noquote()<<"[send msg]"<<msg;
    socketClient->write(msg);
}

void ManageToolClient::sendBytes(QByteArray &bytes)
{
    qDebug()<<"[ManageToolClient::sendBytes]"<<bytes.size();
    if(apiMode() != MODE_STREAM){
        qDebug()<<"[ManageToolClient::sendBytes]"<<"api mode error,current mode is"<<apiMode();
        return;
    }
    socketClient->write(bytes);
}

MTReply* ManageToolClient::serviceReply(QString serviceGroup, QString service, QVariantMap &params, ServiceObject *replySrc)
{
    MTReply* reply = new MTReply(QString(), serviceGroup, service, replySrc);

    reply->setParams(params);
    mapReply.insert(reply->getTimestamp(), reply);

    QByteArray msg = reply->getReplyBytes();
    sendMsg(msg);
//    qDebug() << __func__ << "reply === " << reply;
    return reply;

}

void ManageToolClient::streamUpload(QString fileName)
{
    setApiMode(MODE_STREAM);
    QFile f(fileName);
    if(!f.open(QFile::ReadOnly))
    {
        qDebug()<<"[ManageToolClient::streamUpload] file open failed"<<fileName;
        return;
    }
    QByteArray fileBuf = f.readAll();
    sendBytes(fileBuf);
    setApiMode(MODE_JSON);
}

//void ManageToolClient::streamDownload(QString fileName)
//{

//}
