#ifndef ABSMSGHANDLE_H
#define ABSMSGHANDLE_H

#include <QObject>
#include <QByteArray>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QtDebug>

#include "abssession.h"
class AbsSession;

class AbsMsgHandle : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString key READ key WRITE setKey)
    Q_PROPERTY(int userID READ userID WRITE setUserID NOTIFY userIdChanged)
    Q_PROPERTY(QString name READ name WRITE setName)


public:
    explicit AbsMsgHandle(QObject *parent = nullptr);
    virtual int sendMsg(QByteArray &msg)=0;
    virtual void recvMsg(QByteArray& msg);//接收json消息
    virtual void recvStream(QByteArray& bytes);//接收stream
    virtual void waitForStream(QString filename, QString format, quint64 size);
    virtual void waitForMsg();
    virtual void sendMsgToUser(int id, const QByteArray &msg);
    virtual void sendMsgToUsers(QVariantList ids, const QByteArray &msg);

    void destroy(){emit destroyRequire();};//销毁通信句柄

    enum Mode{
        MODE_JSON,
        MODE_STREAM,
    };
    Q_ENUM(Mode)

    void setApiMode(Mode mode){
        qDebug()<<"[setApiMode]"<<mode;
        m_apiMode = mode;
    };
    Mode apiMode(){
        return m_apiMode;
    };

    const QString &key() const;
    void setKey(const QString &newKey);

    int userID() const;
    void setUserID(int newUserID);

    const QString &name() const;
    void setName(const QString &newName);

private slots:
    virtual void handleCheck();

private:
    /**
     * @brief mapSession
     * 会话映射 [servicePath->session]
     * servicePath=[ServiceGroup]/[service]
     */
    QMap<QString, AbsSession*> mapSession;
    Mode m_apiMode;
    QString m_key;

    //流缓存
    QByteArray streamCache;
    //格式
    QString streamFormat;
    //流签名
    QString streamSign;
    //接收目标字节数
    qint64 streamSize;

    bool saveStream(QString fileName, QByteArray& bytes);

    int m_userID;

    QString m_name;

signals:
    void destroyRequire();//销毁请求
    void streamRst(bool success, QString msg);//流接收结果
    void userIdChanged();
    void msgToUser(int id, QByteArray msg);//向指定id发送msg
};

#endif // ABSMSGHANDLE_H
