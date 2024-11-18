#include "PositionCommand.h"
#include "CanvasContext.h"
#include "GraphicsKernelDefinition.h"
#include "ICanvasScene.h"
#include "PortGraphicsObject.h"
#include "SourceGraphicsObject.h"
#include "SourceNameTextGraphicsObject.h"
#include "SourceProxy.h"
#include "TransformItemGroup.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"

PositionCommand::PositionCommand(ICanvasScene *canvas, QUndoCommand *parent) : CanvasUndoCommand(canvas, parent)
{
    ignoreRedo = true;
}

PositionCommand::~PositionCommand() { }

void PositionCommand::addPositionOffset(QString id, QPointF offset)
{
    if (proxyPositionOrNameChanged.contains(id)) {
        proxyPositionOrNameChanged[id].offest = proxyPositionOrNameChanged[id].offest + offset;
    } else {
        proxyPositionOrNameChanged[id].offest = offset;
    }
    proxyPositionOrNameChanged[id].names = QPair<QString, QString>();
}

void PositionCommand::addnameChanged(QString id, QPair<QString, QString> name)
{
    proxyPositionOrNameChanged[id].offest = QPointF(0, 0);
    proxyPositionOrNameChanged[id].names = name;
}

bool PositionCommand::isEmpty()
{
    return proxyPositionOrNameChanged.isEmpty();
}

void PositionCommand::setConnectorWireContext(QMap<QString, PConnectorWireContext> last,
                                              QMap<QString, PConnectorWireContext> now)
{
    lastConnectorWirePoints = last;
    nowConnectorWirePoints = now;
}

void PositionCommand::undo()
{
    if (!canvasScene) {
        return;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }
    if (canvasScene->getTransformItemGroup()) {
        canvasScene->getTransformItemGroup()->clear();
    }
    QSet<PortGraphicsObject *> outPutPorts;

    QMapIterator<QString, proxyPositionOffsetNamechanged> iter(proxyPositionOrNameChanged);
    while (iter.hasNext()) {
        iter.next();
        QString id = iter.key();
        QPointF offset = iter.value().offest;
        TransformProxyGraphicsObject *proxyGraphics = canvasScene->getTransformProxyGraphicsByID(id);
        if (proxyGraphics) {
            auto sourceProxy = proxyGraphics->getSourceProxy();
            if (!sourceProxy) {
                continue;
            }
            proxyGraphics->moveBy(-offset.x(), -offset.y());
            QPointF proxyPos = proxyGraphics->pos();
            sourceProxy->setPos(proxyPos);
            auto ports = proxyGraphics->getPortGraphicsObjectList();
            for each (auto port in ports) {
                if (port) {
                    auto portContext = port->getPortContext();
                    if (portContext && portContext->type() == PortOutputType) {
                        outPutPorts.insert(port);
                    } else {
                        auto linkWires = port->getLinkedConnectorWireList();
                        for each (auto wire in linkWires) {
                            if (wire) {
                                outPutPorts.insert(wire->getOutputTypePortGraphics());
                            }
                        }
                    }
                }
            }
        }
    }

    QMapIterator<QString, PConnectorWireContext> wireCtxIter(lastConnectorWirePoints);
    QList<QSharedPointer<WireParameters>> wireParametersList;
    while (wireCtxIter.hasNext()) {
        wireCtxIter.next();
        QString key = wireCtxIter.key();
        PConnectorWireContext cacheCtx = wireCtxIter.value();
        PConnectorWireContext ctx = canvasContext->getConnectorWireContext(key);
        if (!ctx) {
            continue;
        }
        ctx->setAutoCalculation(cacheCtx->autoCalculation());
        ConnectorWireGraphicsObject *wireGraphics = canvasScene->getConnectorWireGraphicsByID(key);
        if (wireGraphics) {
            wireGraphics->cleanBranchPoints();
            // 连接线缓存的数据永远是连接线场景坐标为0,0时的坐标点
            wireGraphics->setPos(QPointF(0, 0));
            wireGraphics->setPoints(cacheCtx->points());

            // 有些模块的图片可以变更，端口位置会随之变化，导致撤销后连接线不对，需要特判刷新下
            PSourceProxy src = ctx->srcSource();
            PSourceProxy dst = ctx->dstSource();
            PortGraphicsObject *outPut = wireGraphics->getOutputTypePortGraphics();
            PortGraphicsObject *inPut = wireGraphics->getInputTypePortGraphics();
            auto points = ctx->points();
            if (src && dst && outPut && inPut && !points.isEmpty()) {
                if ((src->prototypeName() == "SeriesReactor"
                     && Utility::lineDistance(points[0], outPut->getLinkPos()) > 0.1)
                    || (dst->prototypeName() == "SeriesReactor"
                        && Utility::lineDistance(points[points.size() - 1], inPut->getLinkPos()) > 0.1)) {
                    wireParametersList.append(wireGraphics->refreshWireParameters());
                }
            }
        }
    }

    canvasScene->refreshConnectorWires(wireParametersList);

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

    for each (auto port in outPutPorts) {
        if (port) {
            port->calcuteConnectorWireBranchPoints();
        }
    }
}

void PositionCommand::redo()
{
    if (ignoreRedo) {
        ignoreRedo = false;
        return;
    }
    if (!canvasScene) {
        return;
    }
    if (canvasScene->getTransformItemGroup()) {
        canvasScene->getTransformItemGroup()->clear();
    }

    QSet<PortGraphicsObject *> outPutPorts;

    QMapIterator<QString, proxyPositionOffsetNamechanged> iter(proxyPositionOrNameChanged);
    while (iter.hasNext()) {
        iter.next();
        QString id = iter.key();
        QPointF offset = iter.value().offest;
        TransformProxyGraphicsObject *proxyGraphics = canvasScene->getTransformProxyGraphicsByID(id);
        if (proxyGraphics) {
            auto sourceProxy = proxyGraphics->getSourceProxy();
            if (!sourceProxy) {
                continue;
            }
            proxyGraphics->moveBy(offset.x(), offset.y());
            QPointF proxyPos = proxyGraphics->pos();
            sourceProxy->setPos(proxyPos);
            auto ports = proxyGraphics->getPortGraphicsObjectList();
            for each (auto port in ports) {
                if (port) {
                    auto portContext = port->getPortContext();
                    if (portContext && portContext->type() == PortOutputType) {
                        outPutPorts.insert(port);
                    } else {
                        auto linkWires = port->getLinkedConnectorWireList();
                        for each (auto wire in linkWires) {
                            if (wire) {
                                outPutPorts.insert(wire->getOutputTypePortGraphics());
                            }
                        }
                    }
                }
            }
        }
    }

    QMapIterator<QString, PConnectorWireContext> wireCtxIter(nowConnectorWirePoints);
    QList<QSharedPointer<WireParameters>> wireParametersList;
    while (wireCtxIter.hasNext()) {
        wireCtxIter.next();
        QString key = wireCtxIter.key();
        PConnectorWireContext cacheCtx = wireCtxIter.value();
        PConnectorWireContext ctx = canvasScene->getCanvasContext()->getConnectorWireContext(key);
        if (ctx) {
            ctx->setAutoCalculation(cacheCtx->autoCalculation());
            ConnectorWireGraphicsObject *wireGraphics = canvasScene->getConnectorWireGraphicsByID(key);
            if (wireGraphics) {
                wireGraphics->cleanBranchPoints();
                // 连接线缓存的数据永远是连接线场景坐标为0,0时的坐标点
                wireGraphics->setPos(QPointF(0, 0));
                wireGraphics->setPoints(cacheCtx->points());

                // 有些模块的图片可以变更，端口位置会随之变化，导致撤销后连接线不对，需要特判刷新下
                PSourceProxy src = ctx->srcSource();
                PSourceProxy dst = ctx->dstSource();
                PortGraphicsObject *outPut = wireGraphics->getOutputTypePortGraphics();
                PortGraphicsObject *inPut = wireGraphics->getInputTypePortGraphics();
                auto points = ctx->points();
                if (src && dst && outPut && inPut && !points.isEmpty()) {
                    if ((src->prototypeName() == "SeriesReactor"
                         && Utility::lineDistance(points[0], outPut->getLinkPos()) > 0.1)
                        || (dst->prototypeName() == "SeriesReactor"
                            && Utility::lineDistance(points[points.size() - 1], inPut->getLinkPos()) > 0.1)) {
                        wireParametersList.append(wireGraphics->refreshWireParameters());
                    }
                }
            }
        }
    }

    canvasScene->refreshConnectorWires(wireParametersList);

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

    for each (auto port in outPutPorts) {
        if (port) {
            port->calcuteConnectorWireBranchPoints();
        }
    }
}

SourceNamePositionCommand::SourceNamePositionCommand(ICanvasScene *canvas, QUndoCommand *parent)
    : PositionCommand(canvas, parent)
{
}

SourceNamePositionCommand::~SourceNamePositionCommand() { }

void SourceNamePositionCommand::undo()
{
    updateNamePosition(true);
}

void SourceNamePositionCommand::redo()
{
    if (ignoreRedo) {
        ignoreRedo = false;
        return;
    }
    updateNamePosition(false);
}

void SourceNamePositionCommand::updateNamePosition(bool undoFlag)
{
    if (!canvasScene) {
        return;
    }

    QMapIterator<QString, proxyPositionOffsetNamechanged> iter(proxyPositionOrNameChanged);
    while (iter.hasNext()) {
        iter.next();
        QString id = iter.key();
        QPointF offset = iter.value().offest;
        QPair<QString, QString> name = iter.value().names;
        TransformProxyGraphicsObject *proxyGraphics = canvasScene->getTransformProxyGraphicsByID(id);
        if (proxyGraphics) {
            QList<QGraphicsItem *> childs = proxyGraphics->childItems();
            for (int i = 0; i < childs.size(); i++) {
                if (childs[i]->type() == kSourceNameTextGraphics) {
                    SourceNameTextGraphicsObject *sourceName = dynamic_cast<SourceNameTextGraphicsObject *>(childs[i]);
                    if (nullptr != sourceName) {
                        if (offset == QPointF(0, 0) && name.first != NULL) {
                            if (undoFlag) {
                                sourceName->seTextNameChanged(name.first, true);
                            } else {
                                sourceName->seTextNameChanged(name.second, true);
                            }
                        } else {
                            int angle = proxyGraphics->getAngle();
                            if (undoFlag) {
                                sourceName->movePositionBy(angle, -offset);
                            } else {
                                sourceName->movePositionBy(angle, offset);
                            }
                        }
                    }
                }
            }
        }
    }
}
