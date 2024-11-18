#pragma once

#include <QObject>

class JSChannel : public QObject
{
    Q_OBJECT

public:
    JSChannel(QObject *parent = nullptr);

    void sendMessageToJS(QString msg);

signals:
    void notifyMsg(QString msg);

public slots:
    void processMessageFromJS(QString msg);
};