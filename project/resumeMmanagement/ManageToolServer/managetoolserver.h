#ifndef MANAGETOOLSERVER_H
#define MANAGETOOLSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QMap>
#include <QString>

#include "ReplyAndService/sockethandle.h"

class ManageToolServer : public QObject
{
    Q_OBJECT
public:
    explicit ManageToolServer(QObject *parent = nullptr);

signals:

private:
    //SOCKET
    QTcpServer * socketServer;
    void initSocketServer();//初始化socket服务端
    void registSessions();

    //....

    //POOL
    QMap<int, AbsMsgHandle*> msgPool;//消息连接池

private slots:
    //SOCKET
    void newSocketConnect();//新的socket连接
    void deleteSocketConnect();//释放socket连接
    void regConnectID();//注册连接ID
    void sendMsg(int id, QByteArray msg);//向客户端id发送msg

};

#endif // MANAGETOOLSERVER_H
