#include "DeleteCommand.h"
#include "CanvasContext.h"
#include "GraphicsKernelDefinition.h"
#include "PortGraphicsObject.h"
#include "SourceGraphicsObject.h"
#include "TransformItemGroup.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"

DeleteCommand::DeleteCommand(ICanvasScene *scene, QUndoCommand *parent /* = 0 */) : CanvasUndoCommand(scene, parent) { }

void DeleteCommand::addDeleteGraphics(GraphicsLayer *layer)
{
    if (nullptr == layer) {
        return;
    }
    int graphicsType = layer->type();
    if (graphicsType == kTransformProxyGraphics) {
        TransformProxyGraphicsObject *graphics = dynamic_cast<TransformProxyGraphicsObject *>(layer);
        if (graphics) {
            PSourceProxy sourceProxy = graphics->getSourceProxy();
            // add by liwenyu 2023.06.09 这里把originalUUID设置为自身的uuid是为了兼容删除之后的撤销重做
            // sourceProxy->setOriginalUUID(sourceProxy->uuid());
            // end
            addSourceProxy(sourceProxy);

            // 删除资源的时候，需要把和该资源相关联的连接线也删除,若连接线上的端口为动态删除，则一并删除
            // 获取该资源的端口列表
            QList<PortGraphicsObject *> portGraphicsList = graphics->getPortGraphicsObjectList();
            // 遍历端口,获取端口的连接线
            QListIterator<PortGraphicsObject *> iterPort(portGraphicsList);
            while (iterPort.hasNext()) {
                PortGraphicsObject *portGraphics = iterPort.next();
                if (nullptr != portGraphics) {
                    // 端口连接线列表
                    QList<ConnectorWireGraphicsObject *> connectorWireList = portGraphics->getLinkedConnectorWireList();
                    QListIterator<ConnectorWireGraphicsObject *> iterConnector(connectorWireList);
                    while (iterConnector.hasNext()) {
                        ConnectorWireGraphicsObject *connectorWireGraphics = iterConnector.next();
                        if (connectorWireGraphics) {
                            PConnectorWireContext connectorCtx = connectorWireGraphics->getConnectorWireContext();

                            addConnectorWireContext(connectorCtx);
                            addConnectWirePort(connectorCtx);
                        }
                    }

                    // 模块的端口可删除
                    if (sourceProxy->isSupportAddOrDelPort()) {
                        addPortContext(portGraphics->getPortContext(), sourceProxy->uuid());
                    }
                }
            }
        }
    } else if (graphicsType == kConnectorWireGraphics) {
        ConnectorWireGraphicsObject *graphics = dynamic_cast<ConnectorWireGraphicsObject *>(layer);
        if (graphics) {
            PConnectorWireContext connectorCtx = graphics->getConnectorWireContext();

            addConnectorWireContext(connectorCtx);
            addConnectWirePort(connectorCtx);
        }
    } else if (graphicsType == kConnectorWireSegment) {
        ConnectorWireGraphicsObject *graphics = dynamic_cast<ConnectorWireGraphicsObject *>(layer->parentItem());
        if (graphics) {
            PConnectorWireContext connectorCtx = graphics->getConnectorWireContext();
            addConnectorWireContext(connectorCtx);
            addConnectWirePort(connectorCtx);
        }
    }
}

void DeleteCommand::addSourceProxy(PSourceProxy source)
{
    if (source.isNull()) {
        return;
    }
    QString uuid = source->uuid();
    if (sourcesMap.contains(uuid)) {
        return;
    }
    sourcesMap[uuid] = source;
}

void DeleteCommand::addConnectorWireContext(PConnectorWireContext connectorCtx)
{
    if (connectorCtx.isNull()) {
        return;
    }
    QString uuid = connectorCtx->uuid();
    if (connectorsMap.contains(uuid)) {
        return;
    }
    connectorsMap[uuid] = connectorCtx;
}

void DeleteCommand::addPortContext(PPortContext portCtx, QString sourceUUID)
{
    if (portCtx.isNull()) {
        return;
    }
    QString uuid = sourceUUID + portCtx->uuid();
    if (portMap.contains(uuid)) {
        return;
    }
    portMap[uuid] = portCtx;
}

void DeleteCommand::addConnectWirePort(PConnectorWireContext connectorCtx)
{
    // 若连接线存在可删除端口，则删除该端口
    PSourceProxy srcSourceProxy = connectorCtx->srcSource();
    PSourceProxy dstSourceProxy = connectorCtx->dstSource();
    if (srcSourceProxy && srcSourceProxy->isSupportAddOrDelPort()) {
        PPortContext portContext = srcSourceProxy->getPortContextByID(connectorCtx->srcPortID());
        addPortContext(portContext, srcSourceProxy->uuid());
    }
    if (dstSourceProxy && dstSourceProxy->isSupportAddOrDelPort()) {
        PPortContext portContext = dstSourceProxy->getPortContextByID(connectorCtx->dstPortID());
        addPortContext(portContext, dstSourceProxy->uuid());
    }
}

void DeleteCommand::redo()
{
    if (ignoreRedo) {
        ignoreRedo = false;
        startVerifyCanvas();
        return;
    }
    if (!canvasScene) {
        return;
    }
    auto itemGroup = canvasScene->getTransformItemGroup();
    QMapIterator<QString, PSourceProxy> iter(sourcesMap);
    while (iter.hasNext()) {
        iter.next();
        QString uuid = iter.key();
        PSourceProxy source = iter.value();
        source->setReadyDeleteFlag(true);
    }
    // 先删除连接线
    QList<PortGraphicsObject *> outputPorts;
    QMapIterator<QString, PConnectorWireContext> iterConnector(connectorsMap);
    while (iterConnector.hasNext()) {
        iterConnector.next();
        QString uuid = iterConnector.key();
        ConnectorWireGraphicsObject *connectorGraphics = canvasScene->getConnectorWireGraphicsByID(uuid);
        if (connectorGraphics) {
            if (itemGroup) {
                itemGroup->removeConnectorWire(connectorGraphics);
            }
            outputPorts.push_back(connectorGraphics->getOutputTypePortGraphics());
            canvasScene->deleteConnectorWireGraphicsObject(connectorGraphics);
        }
    }

    iter.toFront();
    while (iter.hasNext()) {
        iter.next();
        QString uuid = iter.key();
        PSourceProxy source = iter.value();
        source->setReadyDeleteFlag(false);
        TransformProxyGraphicsObject *proxyGraphics = canvasScene->getTransformProxyGraphicsBySource(source);
        canvasScene->deleteTransformProxyGraphicsObject(proxyGraphics);
    }

    // 删除端口(在删除链接线时已经处理了
    startVerifyCanvas();
    //canvasScene->refreshInputAndOutPutIndex();
}

void DeleteCommand::undo()
{
    if (!canvasScene) {
        return;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }
    // 先创建资源
    QMapIterator<QString, PSourceProxy> iter(sourcesMap);
    while (iter.hasNext()) {
        iter.next();
        QString uuid = iter.key();
        PSourceProxy source = iter.value();
        source->setSourceCreateType(SourceProxy::kCreateRevert);
        canvasScene->loadFromSourceProxy(source);
    }

    // 创建端口
    QMapIterator<QString, PPortContext> iterPort(portMap);
    while (iterPort.hasNext()) {
        iterPort.next();
        PPortContext portContext = iterPort.value();

        QString sourceID = portContext->sourceUUID();
        auto pTransformProxy = canvasScene->getTransformProxyGraphicsByID(sourceID);
        if (!pTransformProxy)
            continue;
        SourceGraphicsObject *sourGraphics = pTransformProxy->getSourceGraphicsObject();
        sourGraphics->addPortGraphicsObject(portContext);
    }

    // 再创建连接线
    QList<PortGraphicsObject *> outputPorts;
    QMapIterator<QString, PConnectorWireContext> iterConnector(connectorsMap);
    while (iterConnector.hasNext()) {
        iterConnector.next();
        QString uuid = iterConnector.key();
        PConnectorWireContext connectorCtx = iterConnector.value();

        if (canvasScene->getConnectorWireGraphicsByID(uuid) != nullptr) {
            continue;
        }
        ConnectorWireGraphicsObject *graphics = new ConnectorWireGraphicsObject(canvasScene);
        graphics->loadFromContext(connectorCtx);
        // 连接成功，把连接线上下文添加到画板,这里需要触发数据同步
        if (graphics->isConnectedWithPort()) {
            canvasContext->addConnectWireContext(connectorCtx);
            outputPorts.push_back(graphics->getOutputTypePortGraphics());
        }
    }
    for each (auto port in outputPorts) {
        if (port) {
            port->calcuteConnectorWireBranchPoints();
        }
    }
    startVerifyCanvas();
    //canvasScene->refreshInputAndOutPutIndex();
}
