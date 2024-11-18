#include "sockethandle.h"
#include <QtDebug>
#include <QHostAddress>

SocketHandle::SocketHandle(QTcpSocket *socket, QObject *parent):
    AbsMsgHandle(parent),
    skt(socket)
{
    connect(skt, &QTcpSocket::stateChanged, this, &SocketHandle::stateChanged);
    connect(skt, &QTcpSocket::readyRead, this, &SocketHandle::readMsg);
}

QTcpSocket *SocketHandle::socket()
{
    return skt;
}

void SocketHandle::stateChanged(QTcpSocket::SocketState state)
{
    qDebug()<<"SocketHandle::stateChanged:"<<skt->peerAddress()<<state;
    if(state==QTcpSocket::UnconnectedState){
        emit destroyRequire();
    }
}

void SocketHandle::readMsg()
{
    QByteArray msg = skt->readAll();
//    qDebug()<<"apiMode"<<apiMode();
    if(apiMode() == MODE_JSON)
        recvMsg(msg);
    else
        recvStream(msg);
}

int SocketHandle::sendMsg(QByteArray &msg)
{
    return skt->write(msg);
}
