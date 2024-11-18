#ifndef MANAGETOOLCLIENT_H
#define MANAGETOOLCLIENT_H

#include <QObject>
#include <QMap>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include "mtreply.h"
#include "ReplyAndService/ServiceDefines.h"
#include "Base/singleton.h"
#include "serviceobject.h"
#include "Model/usermodel.h"

class ManageToolClient : public QObject,public Singleton<ManageToolClient>
{
    Q_OBJECT
    friend class Singleton<ManageToolClient>;
private:
    explicit ManageToolClient(QObject *parent = nullptr);

public:
    enum Mode{
        MODE_JSON,
        MODE_STREAM,
    };
    Q_ENUM(Mode)

    void setApiMode(Mode mode){
        m_apiMode = mode;
    };
    Mode apiMode(){
        return m_apiMode;
    };

    void setUserInfo(QVariantMap info);
    UserModel* getUserInfo();


public slots:
    void waitForStream(QString sign, QString format, quint64 size);
    MTReply *serviceReply(QString serviceGroup, QString service, QVariantMap& params, ServiceObject* replySrc);
    //上传文件流
    void streamUpload(QString fileName);
    //下载文件流
//    void streamDownload(QString fileName);

private:
    void initSocket();
    void connectToServer(QString serverAddr, quint16 serverPort);
    void connectToServer(QHostAddress serverAddr, quint16 serverPort);
    void recvMsg(QByteArray& msg);
    void recvStream(QByteArray& bytes);
    void recvNotify(QVariantMap &msgMap);
    void waitForMsg();

    /**
     * @brief mapReply [接口时间戳]->[reply]
     */
    QMap<qint64, MTReply*> mapReply;
    QTcpSocket* socketClient;
    QString userKey;
    Mode m_apiMode;

    UserModel userInfo;//用户信息


    QFile* fCache;
    //流缓存
    QByteArray streamCache;
    //流签名
    QString streamSign;
    //流格式
    QString streamFormat;
    //接收目标字节数
    qint64 streamSize;
    bool saveStream(QString fileName, QByteArray& bytes);

private slots:
    void stateChanged(QTcpSocket::SocketState state);
    void readMsg();
    void sendMsg(QByteArray &msg);
    void sendBytes(QByteArray& bytes);

signals:
    void newArchiveStream(QByteArray& bytes);//新的档案数据流，用于简历预览
    void streamRst(bool success, QString msg);//数据流接收结果
    void newNotify(QString topic, QString msg);//新的通知
    void connectRecover();//连接恢复

//    void login_fail();
//    void login_success();
};

#endif // MANAGETOOLCLIENT_H
