#include "ConnectorWireCommand.h"
#include "CanvasContext.h"
#include "ConnectorWireGraphicsObject.h"
#include "SourceGraphicsObject.h"
#include "SourceProxy.h"
#include "TransformItemGroup.h"
#include "Utility.h"

AddConnectorWireCommand::AddConnectorWireCommand(ICanvasScene *canvas, QUndoCommand *parent)
    : CanvasUndoCommand(canvas, parent)
{
    ignoreRedo = true;
}

void AddConnectorWireCommand::addConnectorWireContext(PConnectorWireContext ctx)
{
    if (ctx.isNull()) {
        return;
    }
    connectors[ctx->uuid()] = ctx;

    // 若连接线存在可删除端口，则删除该端口
    PSourceProxy srcSourceProxy = ctx->srcSource();
    PSourceProxy dstSourceProxy = ctx->dstSource();
    if (srcSourceProxy && srcSourceProxy->isSupportAddOrDelPort()) {
        PPortContext portContext = srcSourceProxy->getPortContextByID(ctx->srcPortID());
        addPortContext(portContext, srcSourceProxy->uuid());
    }
    if (dstSourceProxy && dstSourceProxy->isSupportAddOrDelPort()) {
        PPortContext portContext = dstSourceProxy->getPortContextByID(ctx->dstPortID());
        addPortContext(portContext, dstSourceProxy->uuid());
    }
}

void AddConnectorWireCommand::undo()
{
    deleteConnectorWires();
    startVerifyCanvas();
}

void AddConnectorWireCommand::redo()
{

    if (ignoreRedo) {
        ignoreRedo = false;
        startVerifyCanvas();
        return;
    }
    createConnectorWires();
    startVerifyCanvas();
}

void AddConnectorWireCommand::createConnectorWires()
{
    if (!canvasScene) {
        return;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
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

    // 才能创建链接线，不然找不到端口对象
    QMapIterator<QString, PConnectorWireContext> iter(connectors);
    while (iter.hasNext()) {
        iter.next();
        QString id = iter.key();
        if (canvasScene->getConnectorWireGraphicsByID(id) != nullptr) {
            continue;
        }
        PConnectorWireContext ctx = iter.value();
        ConnectorWireGraphicsObject *graphics = new ConnectorWireGraphicsObject(canvasScene);
        graphics->loadFromContext(ctx);

        // 连接成功，把连接线上下文添加到画板
        canvasContext->addConnectWireContext(ctx);
    }
}

void AddConnectorWireCommand::deleteConnectorWires()
{
    if (!canvasScene) {
        return;
    }
    auto transformItemGroup = canvasScene->getTransformItemGroup();
    if (transformItemGroup) {
        transformItemGroup->clear();
    }
    QMapIterator<QString, PConnectorWireContext> iter(connectors);
    while (iter.hasNext()) {
        iter.next();
        QString id = iter.key();
        ConnectorWireGraphicsObject *connectorGraphics = canvasScene->getConnectorWireGraphicsByID(id);
        if (connectorGraphics) {
            canvasScene->deleteConnectorWireGraphicsObject(connectorGraphics);
        }
    }
}

void AddConnectorWireCommand::addPortContext(PPortContext portCtx, QString sourceUUID)
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

/**********************************ConnectorWireSwitchCommand*******************************************/

ConnectorWireSwitchCommand::ConnectorWireSwitchCommand(ICanvasScene *canvas, PConnectorWireContext context,
                                                       bool switchValue, QUndoCommand *parent /* = 0 */)
    : CanvasUndoCommand(canvas, parent)
{
    this->swtichValue = switchValue;
    this->connectorContext = context;
}

void ConnectorWireSwitchCommand::redo()
{
    if (connectorContext.isNull() || !canvasScene) {
        return;
    }
    QString id = connectorContext->uuid();
    ConnectorWireGraphicsObject *connectorGraphics = canvasScene->getConnectorWireGraphicsByID(id);
    if (connectorGraphics) {
        connectorGraphics->doSwitch(swtichValue);
    }
    // startVerifyCanvas();
}

void ConnectorWireSwitchCommand::undo()
{
    if (connectorContext.isNull() || !canvasScene) {
        return;
    }
    QString id = connectorContext->uuid();
    ConnectorWireGraphicsObject *connectorGraphics = canvasScene->getConnectorWireGraphicsByID(id);
    if (connectorGraphics) {
        connectorGraphics->doSwitch(!swtichValue);
    }
    // startVerifyCanvas();
}

ConnectorWireAdjustCommand::ConnectorWireAdjustCommand(ICanvasScene *canvas, QString wireId,
                                                       PConnectorWireContext oldContext,
                                                       PConnectorWireContext newContext, QUndoCommand *parent)
    : CanvasUndoCommand(canvas, parent)
{
    ignoreRedo = true;
    this->oldWireContext = oldContext;
    this->newWireContext = newContext;
    this->connectorWireId = wireId;
}

void ConnectorWireAdjustCommand::redo()
{
    if (ignoreRedo) {
        ignoreRedo = false;
        return;
    }
    if (!canvasScene || oldWireContext.isNull() || newWireContext.isNull()) {
        return;
    }
    ConnectorWireGraphicsObject *wire = canvasScene->getConnectorWireGraphicsByID(connectorWireId);
    if (!wire) {
        return;
    }
    auto outputPortGraphics = wire->getOutputTypePortGraphics();
    if (outputPortGraphics) {
        outputPortGraphics->cleanConnectorWireBranchPoints();

        ConnectorWireContext *wireCtx = wire->getConnectorWireContext().data();
        *wireCtx = *newWireContext;
        wire->setPos(QPointF(0, 0));
        wire->setPoints(wireCtx->points());

        outputPortGraphics->calcuteConnectorWireBranchPoints();
    }
}

void ConnectorWireAdjustCommand::undo()
{
    if (!canvasScene || oldWireContext.isNull() || newWireContext.isNull()) {
        return;
    }
    ConnectorWireGraphicsObject *wire = canvasScene->getConnectorWireGraphicsByID(connectorWireId);
    if (!wire) {
        return;
    }
    auto outputPortGraphics = wire->getOutputTypePortGraphics();
    if (outputPortGraphics) {
        outputPortGraphics->cleanConnectorWireBranchPoints();

        auto wireCtx = wire->getConnectorWireContext().data();
        *wireCtx = *oldWireContext;
        wire->setPos(QPointF(0, 0));
        wire->setPoints(wireCtx->points());

        outputPortGraphics->calcuteConnectorWireBranchPoints();
    }
}
