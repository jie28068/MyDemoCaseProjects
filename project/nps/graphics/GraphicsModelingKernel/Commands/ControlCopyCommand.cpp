#include "ControlCopyCommand.h"
#include "CanvasContext.h"
#include "TransformItemGroup.h"

ControlCopyCommand::ControlCopyCommand(ICanvasScene *scene, QUndoCommand *parent /* = 0 */)
    : CanvasUndoCommand(scene, parent)
{
    ignoreRedo = true;
}

ControlCopyCommand::~ControlCopyCommand() { }

void ControlCopyCommand::addSourceProxy(PSourceProxy source)
{
    if (source.isNull()) {
        return;
    }
    QString uuid = source->uuid();
    if (sourcesMap.contains(uuid)) {
        return;
    }
    sourcesMap[uuid] = source;
    if (source->isSupportAddOrDelPort()) {
        portContextMap[uuid] = source->portList();
    }
}

void ControlCopyCommand::addConnectorWire(PConnectorWireContext wireContext)
{
    if (wireContext.isNull()) {
        return;
    }
    QString uuid = wireContext->uuid();
    if (connectorWiresMap.contains(uuid)) {
        return;
    }
    connectorWiresMap[uuid] = wireContext;
}

void ControlCopyCommand::createSources()
{
    if (!canvasScene) {
        return;
    }
    QMapIterator<QString, PSourceProxy> iter(sourcesMap);
    while (iter.hasNext()) {
        iter.next();
        QString uuid = iter.key();
        PSourceProxy source = iter.value();
        if (portContextMap.contains(uuid)) {
            source->resetPortList(portContextMap[uuid]);
        }
        source->setSourceCreateType(SourceProxy::kCreateRevert);
        canvasScene->loadFromSourceProxy(source);
    }
}

void ControlCopyCommand::deleteSources()
{
    if (!canvasScene) {
        return;
    }
    QMapIterator<QString, PSourceProxy> iter(sourcesMap);
    while (iter.hasNext()) {
        iter.next();
        QString uuid = iter.key();
        PSourceProxy source = iter.value();
        TransformProxyGraphicsObject *proxyGraphics = canvasScene->getTransformProxyGraphicsBySource(source);
        canvasScene->deleteTransformProxyGraphicsObject(proxyGraphics);
    }
}

void ControlCopyCommand::createConnectorWires()
{
    if (!canvasScene) {
        return;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }
    // 创建连接线图元
    QMapIterator<QString, PConnectorWireContext> newConnectorIter(connectorWiresMap);
    while (newConnectorIter.hasNext()) {
        newConnectorIter.next();
        QString uuid = newConnectorIter.key();
        PConnectorWireContext newConnectorCtx = newConnectorIter.value();

        if (canvasScene->getConnectorWireGraphicsByID(uuid) != nullptr) {
            continue;
        }
        ConnectorWireGraphicsObject *graphics = new ConnectorWireGraphicsObject(canvasScene);
        graphics->loadFromContext(newConnectorCtx);

        if (graphics->isConnectedWithPort()) {
            // 连接成功，把连接线上下文添加到画板
            canvasContext->addConnectWireContext(newConnectorCtx);
        }
    }
}

void ControlCopyCommand::deleteConnectorWires()
{
    if (!canvasScene) {
        return;
    }
    QMapIterator<QString, PConnectorWireContext> iterConnector(connectorWiresMap);
    while (iterConnector.hasNext()) {
        iterConnector.next();
        QString uuid = iterConnector.key();
        ConnectorWireGraphicsObject *connectorGraphics = canvasScene->getConnectorWireGraphicsByID(uuid);
        if (connectorGraphics) {
            canvasScene->deleteConnectorWireGraphicsObject(connectorGraphics);
        }
    }
}

void ControlCopyCommand::undo()
{
    if (canvasScene) {
        TransformItemGroup *group = canvasScene->getTransformItemGroup();
        if (group) {
            group->clear();
        }
        deleteConnectorWires();
        deleteSources();
        startVerifyCanvas();
        //canvasScene->refreshInputAndOutPutIndex();
    }
}

void ControlCopyCommand::redo()
{
    if (ignoreRedo) {
        ignoreRedo = false;
        bool isNeedVerify = false;
        QMapIterator<QString, PSourceProxy> iter(sourcesMap);
        while (iter.hasNext()) {
            iter.next();
            PSourceProxy source = iter.value();
            if (source->moduleType() == "electrical") {
                isNeedVerify = true;
                break;
            }
        }
        if (isNeedVerify) {
            startVerifyCanvas();
        }
        return;
    }
    createSources();
    createConnectorWires();
    startVerifyCanvas();
    //canvasScene->refreshInputAndOutPutIndex();
}