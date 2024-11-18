#include "JSChannel.h"
#include "GISServerManager.h"

#include <QDebug>

JSChannel::JSChannel(QObject *parent) : QObject(parent) { }

void JSChannel::sendMessageToJS(QString msg)
{
    emit notifyMsg(msg);
}

void JSChannel::processMessageFromJS(QString msg)
{
    qDebug() << msg;
    GISServerManager &serverManager = GISServerManager::getInstance();
    if (serverManager.projectManagerServer && serverManager.graphicsModelingServer) {
        PModel boardModel = serverManager.projectManagerServer->GetBoardModelByName(msg);
        if (boardModel) {
            serverManager.graphicsModelingServer->openDrawingBoard(boardModel);
        }
    }
}
