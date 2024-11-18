#include "PortPositionCommand.h"
#include "ConnectorWireContext.h"
#include "SourceGraphicsObject.h"

PortPositionCommand::PortPositionCommand(ICanvasScene *canvas, QString proxyGraphicsID, PPortContext oldPortContext,
                                         PPortContext newPortContext,
                                         QList<PConnectorWireContext> oldConnectorWireContexts,
                                         QList<PConnectorWireContext> newConnectorWireContexts, QUndoCommand *parent)
    : CanvasUndoCommand(canvas, parent)

{
    this->proxyGraphicsID = proxyGraphicsID;
    this->oldPortContext = oldPortContext;
    this->oldConnectorWireContexts = oldConnectorWireContexts;
    this->newPortContext = newPortContext;
    this->newConnectorWireContexts = newConnectorWireContexts;
    ignoreRedo = true;
}

void PortPositionCommand::redo()
{
    if (ignoreRedo) {
        ignoreRedo = false;
        return;
    }

    if (!canvasScene) {
        return;
    }
    PortGraphicsObject *targetPortObj = nullptr;
    auto proxyGraphics = canvasScene->getTransformProxyGraphicsByID(proxyGraphicsID);

    if (proxyGraphics) {
        targetPortObj = proxyGraphics->getPortGraphicsObject(newPortContext->uuid());
    }

    if (!targetPortObj)
        return;

    if (targetPortObj) {
        PortContext *portContext = targetPortObj->getPortContext().data();
        *portContext = *newPortContext;
        targetPortObj->updatePosition();

        PortGraphicsObject *outputPort = targetPortObj->isOutputPort() ? targetPortObj : nullptr;

        for each (auto ctx in newConnectorWireContexts) {
            if (ctx) {
                auto wire = canvasScene->getConnectorWireGraphicsByID(ctx->uuid());
                auto wireCtx = wire->getConnectorWireContext();
                *wireCtx = *ctx;
                wire->setPos(QPointF(0, 0));
                wire->setPoints(wireCtx->points());
                if (!outputPort) {
                    outputPort = wire->getOutputTypePortGraphics();
                }
            }
        }
        if (outputPort) {
            outputPort->calcuteConnectorWireBranchPoints();
        }
    }
}

void PortPositionCommand::undo()
{
    if (!canvasScene) {
        return;
    }
    if (!oldPortContext)
        return;

    PortGraphicsObject *targetPortObj = nullptr;
    auto proxyGraphics = canvasScene->getTransformProxyGraphicsByID(proxyGraphicsID);

    if (proxyGraphics) {
        targetPortObj = proxyGraphics->getPortGraphicsObject(newPortContext->uuid());
    }

    if (targetPortObj) {
        PortContext *portContext = targetPortObj->getPortContext().data();
        *portContext = *oldPortContext;
        targetPortObj->updatePosition();

        PortGraphicsObject *outputPort = targetPortObj->isOutputPort() ? targetPortObj : nullptr;

        for each (auto ctx in oldConnectorWireContexts) {
            if (ctx) {
                auto wire = canvasScene->getConnectorWireGraphicsByID(ctx->uuid());
                auto wireCtx = wire->getConnectorWireContext();
                *wireCtx = *ctx;
                wire->setPos(QPointF(0, 0));
                wire->setPoints(wireCtx->points());
                if (!outputPort) {
                    outputPort = wire->getOutputTypePortGraphics();
                }
            }
        }
        if (outputPort) {
            outputPort->calcuteConnectorWireBranchPoints();
        }
    }
}
