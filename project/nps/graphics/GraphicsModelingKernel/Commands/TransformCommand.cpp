#include "TransformCommand.h"
#include "CanvasContext.h"
#include "ConnectorWireGraphicsObject.h"
#include "PortGraphicsObject.h"

TransformCommand::TransformCommand(ICanvasScene *scene, QUndoCommand *parent) : CanvasUndoCommand(scene, parent)
{
    ignoreRedo = true;
}

void TransformCommand::undo()
{
    if (!canvasScene) {
        return;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }
    QMapIterator<QString, TransformInfo> iter(transformProxyMap);
    while (iter.hasNext()) {
        iter.next();
        QString id = iter.key();
        TransformInfo info = iter.value();
        TransformProxyGraphicsObject *proxy = canvasScene->getTransformProxyGraphicsByID(id);
        if (proxy) {
            proxy->applyTransforms(info.oldScaleTransform, info.oldRotateTransform, info.oldTranslateTransform,
                                   info.oldxFlipTransform, info.oldyFlipTransform);
        }
    }

    QMapIterator<QString, PConnectorWireContext> wireCtxIter(lastConnectorWirePoints);
    while (wireCtxIter.hasNext()) {
        wireCtxIter.next();
        QString key = wireCtxIter.key();
        PConnectorWireContext cacheCtx = wireCtxIter.value();
        PConnectorWireContext ctx = canvasContext->getConnectorWireContext(key);
        if (!ctx || !cacheCtx) {
            continue;
        }
        ctx->setAutoCalculation(cacheCtx->autoCalculation());
        ConnectorWireGraphicsObject *wireGraphics = canvasScene->getConnectorWireGraphicsByID(key);
        if (wireGraphics) {
            wireGraphics->cleanBranchPoints();
            // 连接线缓存的数据永远是连接线场景坐标为0,0时的坐标点
            // wireGraphics->setPos(QPointF(0, 0));
            wireGraphics->setPoints(cacheCtx->points());
        }
    }

    wireCtxIter.toFront();
    // 重置分叉点
    while (wireCtxIter.hasNext()) {
        wireCtxIter.next();
        QString key = wireCtxIter.key();
        ConnectorWireGraphicsObject *wireGraphics = canvasScene->getConnectorWireGraphicsByID(key);
        if (wireGraphics) {
            auto outputPortGraphics = wireGraphics->getOutputTypePortGraphics();
            if (outputPortGraphics) {
                outputPortGraphics->cleanConnectorWireBranchPoints();
                outputPortGraphics->calcuteConnectorWireBranchPoints();
            }
        }
    }
}

void TransformCommand::redo()
{
    if (ignoreRedo) {
        ignoreRedo = false;
        return;
    }
    if (!canvasScene) {
        return;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }
    QMapIterator<QString, TransformInfo> iter(transformProxyMap);
    while (iter.hasNext()) {
        iter.next();
        QString id = iter.key();
        TransformInfo info = iter.value();
        TransformProxyGraphicsObject *proxy = canvasScene->getTransformProxyGraphicsByID(id);
        if (proxy) {
            proxy->applyTransforms(info.newScaleTransform, info.newRotateTransform, info.newTranslateTransform,
                                   info.newxFlipTransform, info.newyFlipTransform);
        }
    }

    QMapIterator<QString, PConnectorWireContext> wireCtxIter(nowConnectorWirePoints);
    while (wireCtxIter.hasNext()) {
        wireCtxIter.next();
        QString key = wireCtxIter.key();
        PConnectorWireContext cacheCtx = wireCtxIter.value();
        PConnectorWireContext ctx = canvasContext->getConnectorWireContext(key);
        if (!ctx || !cacheCtx) {
            continue;
        }
        ctx->setAutoCalculation(cacheCtx->autoCalculation());
        ConnectorWireGraphicsObject *wireGraphics = canvasScene->getConnectorWireGraphicsByID(key);
        if (wireGraphics) {
            wireGraphics->cleanBranchPoints();
            wireGraphics->setPoints(cacheCtx->points());
        }
    }

    wireCtxIter.toFront();
    // 重置分叉点
    while (wireCtxIter.hasNext()) {
        wireCtxIter.next();
        QString key = wireCtxIter.key();
        ConnectorWireGraphicsObject *wireGraphics = canvasScene->getConnectorWireGraphicsByID(key);
        if (wireGraphics) {
            auto outputPortGraphics = wireGraphics->getOutputTypePortGraphics();
            if (outputPortGraphics) {
                outputPortGraphics->cleanConnectorWireBranchPoints();
                outputPortGraphics->calcuteConnectorWireBranchPoints();
            }
        }
    }
}

bool TransformCommand::isEmpty()
{
    return transformProxyMap.isEmpty();
}

void TransformCommand::setOldTransforms(QString id, QTransform scaleTransform, QTransform rotateTransform,
                                        QTransform translateTransform, QTransform xFlipTransform,
                                        QTransform yFlipTransform)
{
    if (transformProxyMap.contains(id)) {
        auto info = transformProxyMap[id];
        info.oldScaleTransform = scaleTransform;
        info.oldRotateTransform = rotateTransform;
        info.oldTranslateTransform = translateTransform;
        info.oldxFlipTransform = xFlipTransform;
        info.oldyFlipTransform = yFlipTransform;
        transformProxyMap[id] = info;
    } else {
        TransformInfo info;
        info.oldScaleTransform = scaleTransform;
        info.oldRotateTransform = rotateTransform;
        info.oldTranslateTransform = translateTransform;
        info.oldxFlipTransform = xFlipTransform;
        info.oldyFlipTransform = yFlipTransform;
        transformProxyMap[id] = info;
    }
}

void TransformCommand::setNewTransforms(QString id, QTransform scaleTransform, QTransform rotateTransform,
                                        QTransform translateTransform, QTransform xFlipTransform,
                                        QTransform yFlipTransform)
{
    if (transformProxyMap.contains(id)) {
        auto info = transformProxyMap[id];
        info.newScaleTransform = scaleTransform;
        info.newRotateTransform = rotateTransform;
        info.newTranslateTransform = translateTransform;
        info.newxFlipTransform = xFlipTransform;
        info.newyFlipTransform = yFlipTransform;
        transformProxyMap[id] = info;
    } else {
        TransformInfo info;
        info.newScaleTransform = scaleTransform;
        info.newRotateTransform = rotateTransform;
        info.newTranslateTransform = translateTransform;
        info.newxFlipTransform = xFlipTransform;
        info.newyFlipTransform = yFlipTransform;
        transformProxyMap[id] = info;
    }
}

void TransformCommand::setConnectorWireContext(QMap<QString, PConnectorWireContext> last,
                                               QMap<QString, PConnectorWireContext> now)
{
    lastConnectorWirePoints = last;
    nowConnectorWirePoints = now;
}
