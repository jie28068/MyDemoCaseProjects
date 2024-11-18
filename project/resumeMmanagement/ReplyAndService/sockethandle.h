#ifndef SOCKETHANDLE_H
#define SOCKETHANDLE_H

#include <QObject>
#include <QTcpSocket>
#include "absmsghandle.h"

class SocketHandle : public AbsMsgHandle
{
    Q_OBJECT
public:
    explicit SocketHandle(QTcpSocket* socket, QObject *parent = nullptr);
    QTcpSocket *socket();

private:
    QTcpSocket* skt;

signals:

private slots:
    void stateChanged(QTcpSocket::SocketState state);
    void readMsg();

    // AbsMsgHandle interface
public:
    int sendMsg(QByteArray &msg);

};

#endif // SOCKETHANDLE_H
