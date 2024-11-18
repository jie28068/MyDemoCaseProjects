#include "TransformItemGroup.h"
#include "Annotation/GeometricGraphicsAnnotation.h"
#include "BuriedData.h"
#include "CanvasContext.h"
#include "CanvasViewDefaultImpl.h"
#include "ConnectorWireContext.h"
#include "ConnectorWireGraphicsObject.h"
#include "ControlCopyCommand.h"
#include "GraphicsKernelDefinition.h"
#include "GraphicsLayer.h"
#include "PositionCommand.h"
#include "SourceProxy.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"

#include <ActionManager.h>
#include <QApplication>
#include <QDebug>
#include <QGraphicsScene>
#include <QPainter>
#include <QtCore>

class TransformItemGroupPrivate
{
public:
    TransformItemGroupPrivate();

    ICanvasScene *canvasScene;

    QPointF lastPos;

    QPointF totalOffset;

    QRectF rc;

    bool isCopying;

    QPointF mousePressedPoint;

    QMap<QString, PSourceProxy> originalSourcesMap;

    QMap<QString, PConnectorWireContext> originalConnectorsMap;

    QMap<QString, PSourceProxy> newSourcesMap;

    QMap<QString, PConnectorWireContext> newConnectorsMap;

    TransformItemGroup *tempGroup;

    QMap<QString, GraphicsLayer *> layersMap;

    QList<ConnectorWireGraphicsObject *> connectorWires;

    QList<TransformProxyGraphicsObject *> tranformProxyGraphics;

    /// @brief 加入到图元组内的连接线输出端口和代理图元内输出端口的集合
    /// 把输出端口的集合保存起来是为了连接线分支点的清理和计算
    QList<PortGraphicsObject *> outputPorts;

    /// @brief 是否清理了连接线分支点
    bool hasCleanWiresBranchPoint;

    TransformProxyGraphicsObject *proxyUnderMouse;

    /// @brief 输入端点集合主要用于判断电气元件母线的端口复制的时候是否需要保留
    QList<PPortContext> inputPorts;
};

TransformItemGroupPrivate::TransformItemGroupPrivate()
{
    lastPos = QPointF(0, 0);
    totalOffset = QPointF(0, 0);
    isCopying = false;
    tempGroup = nullptr;
    hasCleanWiresBranchPoint = false;
    proxyUnderMouse = nullptr;
}

TransformItemGroup::TransformItemGroup(ICanvasScene *canvasScene, QGraphicsItem *parent) : QGraphicsItemGroup(parent)
{
    dataPtr.reset(new TransformItemGroupPrivate());
    dataPtr->canvasScene = canvasScene;
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
}

void TransformItemGroup::clear()
{
    if (dataPtr->canvasScene) {
        bool isTempGroup = false;
        if (this != dataPtr->canvasScene->getTransformItemGroup()) {
            isTempGroup = true;
        }
        for each (auto wire in dataPtr->connectorWires) {
            if (wire) {
                wire->removeFromGroup(this);

                wire->applyOffsetPos();

                wire->setSelected(false);
            }
        }
        dataPtr->connectorWires.clear();
        for each (auto proxyGraphics in dataPtr->tranformProxyGraphics) {
            if (proxyGraphics) {
                proxyGraphics->removeFromGroup(this);
                proxyGraphics->setSelected(false);
            }
        }
        dataPtr->tranformProxyGraphics.clear();

        for each (auto port in dataPtr->outputPorts) {
            if (port) {
                port->calcuteConnectorWireBranchPoints();
            }
        }
        dataPtr->outputPorts.clear();
        dataPtr->inputPorts.clear();

        dataPtr->canvasScene->removeItem(this);
        if (!isTempGroup) {
            dataPtr->canvasScene->setTransformItemGroup(nullptr);
        }
        clearUnderMouseProxy();
    }
}

QVariant TransformItemGroup::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (!dataPtr->canvasScene) {
        return value;
    }
    auto canvasContext = dataPtr->canvasScene->getCanvasContext();
    if (!canvasContext) {
        return value;
    }
    switch (change) {
    case QGraphicsItem::ItemSelectedHasChanged: {
        if (!isSelected()) {
            clear();
        }
    } break;
    case QGraphicsItem::ItemPositionHasChanged: {
        auto childs = childItems();

        QList<QSharedPointer<WireParameters>> wireParametersList;

        for each (auto child in childs) {
            TransformProxyGraphicsObject *transformGraphics = dynamic_cast<TransformProxyGraphicsObject *>(child);
            if (transformGraphics) {
                QList<PortGraphicsObject *> portList = transformGraphics->getPortGraphicsObjectList();
                for each (auto port in portList) {
                    if (port) {
                        auto links = port->getLinkedConnectorWireList();
                        if (!links.isEmpty()) {
                            // 更新端口的坐标，控制端口采用变量保存，图元组的情况下需要清空重置之前保存的变量
                            port->updatePosition();
                        }
                        for each (auto co in links) {
                            if (co && co->topLevelItem() != this) {
                                auto parameterPtr = co->refreshWireParameters();
                                if (parameterPtr) {
                                    wireParametersList.append(parameterPtr);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (!wireParametersList.isEmpty()) {
            if (dataPtr->canvasScene) {
                dataPtr->canvasScene->refreshConnectorWires(wireParametersList);
            }
        }
        createAlignLine();

    } break;

    case QGraphicsItem::ItemPositionChange: {
        if (boundingRect().isEmpty()) {
            return pos();
        }
        if (dataPtr->isCopying) {
            return pos();
        }
        if (dataPtr->canvasScene && !canvasContext->canModify()) {
            // 画板不可以修改，图元组也不可以移动
            return pos();
        }
        if (canvasContext) {
            auto interactioMode = canvasContext->getInteractionMode();
            // 区域放大模式下不允许调节
            if (interactioMode == kAreaAmplification) {
                return pos();
            }
        }
        QPointF movingPos = value.toPointF();
        QPointF alignMovingPos = Utility::pointAlignmentToGrid(movingPos, 10);
        QRectF changingRect =
                QRectF(movingPos + boundingRect().topLeft(), QSizeF(boundingRect().width(), boundingRect().height()));

        // add by liwenyu 2023.07.05 研发需求:https://zt.xtkfpt.online/story-view-143.html
        CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(dataPtr->canvasScene);
        if (defaultScene) {
            QRectF copyRightRect = defaultScene->getCopyRightRect();
            if (canvasContext->getCanvasProperty().isLegendVisible() && changingRect.intersects(copyRightRect)) {
                return pos();
            }
        }
        // end

        // 如果移动到场景外
        auto currentScene = scene();
        /*         if (currentScene && !currentScene->sceneRect().toRect().contains(changingRect.toRect(), true)) {
                    QRectF sceneRect = currentScene->sceneRect();
                    sceneRect = sceneRect.adjusted(5, 5, -5, -5);
                    if (changingRect.top() > sceneRect.top() && changingRect.bottom() < sceneRect.bottom()) {
                        if (changingRect.left() < sceneRect.left()) {
                            alignMovingPos.setX(-boundingRect().left());
                            alignMovingPos = Utility::pointAlignmentToGrid(alignMovingPos, 10);
                            return alignMovingPos;
                        } else if (changingRect.right() > sceneRect.right()) {
                            alignMovingPos.setX(sceneRect.right() - changingRect.width() - boundingRect().left());
                            alignMovingPos = Utility::pointAlignmentToGrid(alignMovingPos, 10);
                            return alignMovingPos;
                        } else {
                            return pos();
                        }
                    } else if (changingRect.left() > sceneRect.left() && changingRect.right() < sceneRect.right()) {
                        if (changingRect.top() < sceneRect.top()) {
                            alignMovingPos.setY(-boundingRect().y());
                            alignMovingPos = Utility::pointAlignmentToGrid(alignMovingPos, 10);
                            return alignMovingPos;
                        } else if (changingRect.bottom() > sceneRect.bottom()) {
                            alignMovingPos.setY(sceneRect.bottom() - changingRect.height() - boundingRect().top());
                            alignMovingPos = Utility::pointAlignmentToGrid(alignMovingPos, 10);
                            return alignMovingPos;
                        } else {
                            return pos();
                        }
                    }
                    return pos();
                } */
        QRectF sceneRect = defaultScene->sceneRect();
        if (changingRect.left() < sceneRect.left()) {
            sceneRect.setLeft(changingRect.left());
            defaultScene->setSceneRect(sceneRect);
        } else if (changingRect.right() > sceneRect.right()) {
            sceneRect.setRight(changingRect.right());
            defaultScene->setSceneRect(sceneRect);
        }

        if (changingRect.top() < sceneRect.top()) {
            sceneRect.setTop(changingRect.top());
            defaultScene->setSceneRect(sceneRect);
        } else if (changingRect.bottom() > sceneRect.bottom()) {
            sceneRect.setBottom(changingRect.bottom());

            defaultScene->setSceneRect(sceneRect);
        }
        // 提前触发位置变更"*"
        canvasContext->canvasGroupPositionChanged();
        return alignMovingPos;

    } break;
    default: {
    }
    }
    return QGraphicsItemGroup::itemChange(change, value);
}

void TransformItemGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    /*     painter->save();
        painter->setPen(QPen(QColor(0x2E9FE6), 1, Qt::SolidLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect());
        painter->restore(); */
}

void TransformItemGroup::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (dataPtr->canvasScene) {
        dataPtr->canvasScene->setTransformProxyChanging(false);
    }
    if (dataPtr->isCopying) {
        dataPtr->isCopying = false;
        if (dataPtr->tempGroup && dataPtr->canvasScene) {
            /*  撤销重做 */
            ControlCopyCommand *command = new ControlCopyCommand(dataPtr->canvasScene);
            QMapIterator<QString, PSourceProxy> newSourceIter(dataPtr->newSourcesMap);
            while (newSourceIter.hasNext()) {
                newSourceIter.next();
                PSourceProxy source = newSourceIter.value();
                // 后面clear()会暂时将所有图元移除组，那边会调用source的setPos这里不用调用了
                /*QString sourceUUID = source->uuid();
                auto proxyGraphics = dataPtr->canvasScene->getTransformProxyGraphicsByID(sourceUUID);
                if (proxyGraphics) {
                    QPointF itemPos = proxyGraphics->mapToScene(proxyGraphics->boundingRect()).boundingRect().center();
                    source->setPos(itemPos);
                } */
                command->addSourceProxy(source);
            }

            QMapIterator<QString, PConnectorWireContext> newConnectorIter(dataPtr->newConnectorsMap);
            while (newConnectorIter.hasNext()) {
                newConnectorIter.next();
                PConnectorWireContext newConnectorCtx = newConnectorIter.value();
                command->addConnectorWire(newConnectorCtx);
            }
            auto undoStack = dataPtr->canvasScene->getUndoStack();
            if (undoStack) {
                undoStack->push(command);
            }

            dataPtr->tempGroup->clearUnderMouseProxy();
            // 清理图元组
            clear();
            // 由于清理之后，会把图元组从场景中移除，此处复制之后，新创建的资源从临时组转到操作的场景组，
            // 重新把当前组加入到场景
            dataPtr->canvasScene->addItem(this);

            // 重置图元组位置
            setPos(QPointF(0, 0));

            // 临时组内的代理图层对象
            auto tempProxyGraphics = dataPtr->tempGroup->getTransformProxyGraphics();

            // 临时组内的连接线对象
            auto tempWires = dataPtr->tempGroup->getConnectorWires();

            // 把临时组清理掉
            dataPtr->tempGroup->clear();

            delete dataPtr->tempGroup;

            ICanvasScene *cacheCanvasScene = dataPtr->canvasScene;

            dataPtr.reset(new TransformItemGroupPrivate());
            dataPtr->canvasScene = cacheCanvasScene;

            for each (auto proxyGraphics in tempProxyGraphics) {
                if (proxyGraphics) {
                    proxyGraphics->refreshSizeControlGraphics(false);
                    proxyGraphics->addToGroup(this);
                }
            }

            for each (auto wire in tempWires) {
                if (wire) {
                    wire->addToGroup(this);
                }
            }

            setSelected(true);

            dataPtr->canvasScene->setTransformItemGroup(this);
        }
        return QGraphicsItemGroup::mouseReleaseEvent(event);
    }

    QPointF currentPos = pos();
    QPointF offset = currentPos - dataPtr->lastPos;
    // dataPtr->totalOffset += offset;
    if (offset != QPointF(0, 0)) {
        dealUndoRedo(offset);
        QRectF rc = boundingRect();
        QRectF changingRect = QRectF(pos() + rc.topLeft(), QSizeF(rc.width(), rc.height()));
        CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(dataPtr->canvasScene);
        if (defaultScene) {
            QRectF rect = defaultScene->getAllItemUnitedRect();
            if (rect.left() > 0) {
                rect.setLeft(0);
            }
            if (rect.top() > 0) {
                rect.setTop(0);
            }
            if (rect.right() < 2239) {
                rect.setRight(2239);
            }
            if (rect.bottom() < 1583) {
                rect.setBottom(1583);
            }
            defaultScene->setSceneRect(rect);
        }
    }
    clearUnderMouseProxy();

    if (dataPtr->hasCleanWiresBranchPoint) {
        for each (auto port in dataPtr->outputPorts) {
            if (port) {
                port->calcuteConnectorWireBranchPoints();
            }
        }
        dataPtr->hasCleanWiresBranchPoint = false;
    }
    QGraphicsItemGroup::mouseReleaseEvent(event);
}

void TransformItemGroup::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (dataPtr->canvasScene) {
        dataPtr->canvasScene->resetSubsystemGraphics();
    }
    dataPtr->lastPos = pos();
    dataPtr->mousePressedPoint = event->scenePos();
    dataPtr->hasCleanWiresBranchPoint = false;

    setUnderMouseProxy();

    if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier) {
        dataPtr->isCopying = true;
        QGraphicsItemGroup::mousePressEvent(event);
        return;
    }
    QGraphicsItemGroup::mousePressEvent(event);
}

void TransformItemGroup::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!dataPtr->canvasScene) {
        return QGraphicsItemGroup::mouseMoveEvent(event);
    }
    auto canvasContext = dataPtr->canvasScene->getCanvasContext();
    if (!canvasContext) {
        return QGraphicsItemGroup::mouseMoveEvent(event);
    }
    if (dataPtr->canvasScene) {
        dataPtr->canvasScene->setTransformProxyChanging(true);
    }
    if (dataPtr->isCopying) {
        QPointF nowScenePos = event->scenePos();
        if (!dataPtr->mousePressedPoint.isNull() && (nowScenePos - dataPtr->mousePressedPoint).manhattanLength() >= 10
            && dataPtr->canvasScene && canvasContext->canModify()) {
            dataPtr->mousePressedPoint = QPointF();
            onTriggerCopy();
            return QGraphicsItemGroup::mouseMoveEvent(event);
        }
    } else {
        if (!dataPtr->hasCleanWiresBranchPoint) {
            for each (auto port in dataPtr->outputPorts) {
                if (port) {
                    port->cleanConnectorWireBranchPoints();
                }
            }
            dataPtr->hasCleanWiresBranchPoint = true;
        }
    }
    QGraphicsItemGroup::mouseMoveEvent(event);
}

QRectF TransformItemGroup::boundingRect() const
{
    if (dataPtr->rc.isEmpty()) {
        dataPtr->rc = shape().controlPointRect();
        dataPtr->rc = dataPtr->rc.translated(-pos());
    }
    return dataPtr->rc;
}

QPainterPath TransformItemGroup::shape() const
{
    QPainterPath path;
    auto childs = childItems();
    for each (auto child in childs) {
        if (child->type() == kTransformProxyGraphics) {
            TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(child);
            if (proxyGraphics) {
                auto sourceGraphicsObject = proxyGraphics->getSourceGraphicsObject();
                if (sourceGraphicsObject && sourceGraphicsObject->type() == kGeometricAnnotationGraphics) {
                    GeometricGraphicsAnnotation *annotationGraphics =
                            dynamic_cast<GeometricGraphicsAnnotation *>(proxyGraphics->getSourceGraphicsObject());
                    if (annotationGraphics) {
                        QPolygonF cachePoints = annotationGraphics->getCachePoints();
                        QPointF sourcePos = proxyGraphics->pos();
                        for (int i = 0; i < cachePoints.size(); i++) {
                            cachePoints[i] = cachePoints[i] + sourcePos;
                        }
                        path.addPolygon(cachePoints);
                    }
                } else {
                    path.addRect(proxyGraphics->mapToScene(proxyGraphics->unitedChildsBoundingRect()).boundingRect());
                }
            }
        }
    }
    QPainterPathStroker stroker;
    path = stroker.createStroke(path);
    return path;
}

void TransformItemGroup::resetBoundingRect()
{
    dataPtr->rc = QRectF();
}

void TransformItemGroup::dealUndoRedo(QPointF offset)
{
    if (dataPtr->canvasScene) {
        QMap<QString, PConnectorWireContext> lastConnectorWireCtx;
        QMap<QString, PConnectorWireContext> refreshedConnectorWireCtx;
        PositionCommand *command = new PositionCommand(dataPtr->canvasScene, 0);
        auto childs = childItems();
        for each (auto child in childs) {
            if (child->type() == kTransformProxyGraphics) {
                TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(child);
                if (proxyGraphics) {
                    QString id = proxyGraphics->id();
                    command->addPositionOffset(id, offset);

                    auto portLists = proxyGraphics->getPortGraphicsObjectList();
                    for each (auto port in portLists) {
                        if (port) {
                            port->calcuteConnectorWireBranchPoints();
                        }
                    }
                }
            } else if (child->type() == kConnectorWireGraphics) {
                ConnectorWireGraphicsObject *connectorWire = dynamic_cast<ConnectorWireGraphicsObject *>(child);
                if (connectorWire) {
                    PConnectorWireContext ctx = connectorWire->getConnectorWireContext();
                    PConnectorWireContext lastCtx = PConnectorWireContext(new ConnectorWireContext(*ctx));
                    connectorWire->removeFromGroup(this);
                    connectorWire->applyOffsetPos();
                    PConnectorWireContext refreshedCtx = PConnectorWireContext(new ConnectorWireContext(*ctx));
                    refreshedCtx->setPos(QPointF(0, 0));
                    lastCtx->setPos(QPointF(0, 0));
                    refreshedConnectorWireCtx[ctx->uuid()] = refreshedCtx;
                    lastConnectorWireCtx[ctx->uuid()] = lastCtx;
                    connectorWire->addToGroup(this);
                }
            }
        }
        command->setConnectorWireContext(lastConnectorWireCtx, refreshedConnectorWireCtx);
        auto undoStack = dataPtr->canvasScene->getUndoStack();
        if (undoStack) {
            undoStack->push(command);
        }
    }
}

void TransformItemGroup::removeConnectorWire(ConnectorWireGraphicsObject *connector)
{
    if (connector) {
        int index = dataPtr->connectorWires.indexOf(connector);
        if (index != -1) {
            connector->removeFromGroup(this);
            connector->applyOffsetPos();
            connector->setSelected(false);
            dataPtr->connectorWires.removeAt(index);
        }
    }
}

void TransformItemGroup::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (dataPtr->canvasScene) {
        auto ctx = dataPtr->canvasScene->getCanvasContext();
        if (ctx) {
            QSharedPointer<QMenu> menu = QSharedPointer<QMenu>(new QMenu());
            ActionManager::getInstance().graphicsGroupMenu(menu.data(), ctx, isExistConnectorWires(),
                                                           isExistControlCode());
            addThroughAndDisableAction(menu);
            menu->exec(event->screenPos());
            event->accept();
        }
    }
}

void TransformItemGroup::addToGroup(QGraphicsItem *item)
{
    int itemType = item->type();
    if (itemType >= kTransformProxyGraphics) {
        GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
        if (layer) {
            QString id = layer->id();
            // 如果图元已经在图元组内，返回
            if (dataPtr->layersMap.contains(id)) {
                return;
            }
            dataPtr->layersMap[id] = layer;
        }
    }

    QGraphicsItemGroup::addToGroup(item);
    // 如果是连接线
    if (item->type() == kConnectorWireGraphics) {
        ConnectorWireGraphicsObject *wire = dynamic_cast<ConnectorWireGraphicsObject *>(item);
        if (wire && dataPtr->connectorWires.indexOf(wire) == -1) {
            dataPtr->connectorWires.push_back(wire);
            PortGraphicsObject *outputPort = wire->getOutputTypePortGraphics();
            if (outputPort) {
                dataPtr->outputPorts.push_back(outputPort);
            }
            PortGraphicsObject *inputPort = wire->getInputTypePortGraphics();
            if (inputPort) {
                dataPtr->inputPorts.push_back(inputPort->getPortContext());
            }
        }
    } else if (item->type() == kTransformProxyGraphics) {
        TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
        if (proxyGraphics && dataPtr->tranformProxyGraphics.indexOf(proxyGraphics) == -1) {
            dataPtr->tranformProxyGraphics.push_back(proxyGraphics);
            auto proxyPorts = proxyGraphics->getPortGraphicsObjectList();
            for each (auto port in proxyPorts) {
                if (port) {
                    QSharedPointer<PortContext> portCtx = port->getPortContext();
                    if (portCtx->type() == PortOutputType) {
                        dataPtr->outputPorts.push_back(port);
                    } else {
                        // 输入端口的连接线
                        QList<ConnectorWireGraphicsObject *> linkWires = port->getLinkedConnectorWireList();
                        for each (auto wire in linkWires) {
                            if (wire) {
                                PortGraphicsObject *outputPort = wire->getOutputTypePortGraphics();
                                if (outputPort) {
                                    dataPtr->outputPorts.push_back(outputPort);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool TransformItemGroup::hasLayer(QString id)
{
    return dataPtr->layersMap.contains(id);
}

QList<ConnectorWireGraphicsObject *> TransformItemGroup::getConnectorWires()
{
    return dataPtr->connectorWires;
}

QList<TransformProxyGraphicsObject *> TransformItemGroup::getTransformProxyGraphics()
{
    return dataPtr->tranformProxyGraphics;
}

bool TransformItemGroup::isExistConnectorWires()
{
    if (!dataPtr->connectorWires.isEmpty()) {
        return true;
    }
    auto proxyLayer = dataPtr->tranformProxyGraphics;
    foreach (auto it, proxyLayer) {
        if (it) {
            QList<PortGraphicsObject *> portList = it->getPortGraphicsObjectList();
            foreach (PortGraphicsObject *port, portList) {
                if (port->getLinkedConnectorWireList().isEmpty()) {
                    continue;
                } else {
                    return true;
                }
            }
        }
    }
    return false;
}

bool TransformItemGroup::isExistControlCode()
{
    if (!dataPtr->canvasScene) {
        return false;
    }
    auto canvasContext = dataPtr->canvasScene->getCanvasContext();
    if (!canvasContext) {
        return false;
    }
    if (canvasContext->type() == CanvasContext::kElectricalType) {
        return false;
    }
    auto businessHook = canvasContext->getBusinessHooksServer();
    if (!businessHook) {
        return false;
    }
    auto proxyLayer = dataPtr->tranformProxyGraphics;
    foreach (auto it, proxyLayer) {
        if (it) {
            auto source = it->getSourceProxy();
            if (businessHook->isBlockCtrlUserCombined(source)) {
                return true;
            }
        }
    }
    return false;
}

void TransformItemGroup::removeFromGroup(QGraphicsItem *item)
{
    int itemType = item->type();
    if (itemType >= kTransformProxyGraphics) {
        GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
        if (layer) {
            QString id = layer->id();
            dataPtr->layersMap.remove(id);
        }
    }
    QGraphicsItemGroup::removeFromGroup(item);
}

void TransformItemGroup::onTriggerCopy()
{
    if (!dataPtr->canvasScene) {
        return;
    }
    PCanvasContext canvasCtx = dataPtr->canvasScene->getCanvasContext();
    if (!canvasCtx) {
        return;
    }
    auto businessHook = canvasCtx->getBusinessHooksServer();

    // 清空剪切板
    QApplication::clipboard()->clear();
    dataPtr->tempGroup = new TransformItemGroup(dataPtr->canvasScene);

    for each (auto proxyGraphics in dataPtr->tranformProxyGraphics) {
        if (proxyGraphics) {
            PSourceProxy originSource = proxyGraphics->getSourceProxy();
            dataPtr->originalSourcesMap[originSource->uuid()] = originSource;
        }
    }

    for each (auto wire in dataPtr->connectorWires) {
        if (wire) {
            PConnectorWireContext originalConnectorCtx = wire->getConnectorWireContext();
            dataPtr->originalConnectorsMap[originalConnectorCtx->uuid()] = originalConnectorCtx;
        }
    }

    QMap<QString, PSourceProxy> oldToNewSourceMap;

    // 新复制的图元和原始图元的坐标偏移量
    QPointF distanceOffset = pos();
    // 粘贴产生的新资源映射表
    if (dataPtr->newSourcesMap.isEmpty()) {
        QMapIterator<QString, PSourceProxy> originalSourceIter(dataPtr->originalSourcesMap);
        QVector<QString> allSourceName;
        QMapIterator<QString, PSourceProxy> originalSourceIters(dataPtr->originalSourcesMap);
        while (originalSourceIters.hasNext()) {
            originalSourceIters.next();
            allSourceName.push_back(originalSourceIters.value().data()->name());
        }
        while (originalSourceIter.hasNext()) {
            originalSourceIter.next();
            QString id = originalSourceIter.key();
            PSourceProxy originalSource = originalSourceIter.value();

            // 把新创建的资源的名称给业务层接口处理,然后改变资源的uuid
            PSourceProxy newSource = PSourceProxy(new SourceProxy(*originalSource.data()));

            // 注释模块只有图片注释有名称 其他都不需要名称
            if (originalSource->moduleType() != GKD::SOURCE_MODULETYPE_ANNOTATION
                || originalSource->prototypeName() == ImageAnnotationStr) {
                if (businessHook) {
                    businessHook->setNewSourceProxy(originalSource, newSource, allSourceName); // 重命名
                }
            }
            newSource->setUUID(Utility::createUUID()); // 更新UUID
            newSource->setOriginalUUID(originalSource->uuid()); // 设置源UUID，表明是从哪一个souce拷贝构造出来的
            newSource->setPos(originalSource->pos() + distanceOffset); // 设置坐标

            // 重新设置输入输出序列
            if (newSource->prototypeName() == "In" || newSource->prototypeName() == "Out") {
                SourceProperty &property = newSource->getSourceProperty();
                property.setInputOutputIndex(-1);
                property.setInputOutputDisplayIndex(0);
            }

            // 母线复制的时候要删去不在组内的连接线对应的端口
            if (newSource->prototypeName() == "Busbar" || newSource->prototypeName() == "DotBusbar") {
                QList<QSharedPointer<PortContext>> portList;
                for each (auto port in originalSource->portList()) {
                    if (dataPtr->inputPorts.contains(port)) {
                        portList.append(PPortContext(new PortContext(*port.data())));
                    }
                }
                newSource->resetPortList(portList);
            }

            newSource->resetPortSourceUUid();

            dataPtr->newSourcesMap[newSource->uuid()] = newSource; // 把新创建的资源添加到map映射表
            oldToNewSourceMap[originalSource->uuid()] = newSource; // 保存新旧数据的映射关系
        }
    }

    if (dataPtr->newConnectorsMap.isEmpty()) {
        QMapIterator<QString, PConnectorWireContext> originalConnectorIter(dataPtr->originalConnectorsMap);
        while (originalConnectorIter.hasNext()) {
            originalConnectorIter.next();
            PConnectorWireContext originalConnectorCtx = originalConnectorIter.value();
            PSourceProxy srcSource = originalConnectorCtx->srcSource();
            PSourceProxy dstSource = originalConnectorCtx->dstSource();

            if (!srcSource.isNull() && !dstSource.isNull()) {
                QString srcUUID = srcSource->uuid();
                QString dstUUID = dstSource->uuid();
                if (oldToNewSourceMap.contains(srcUUID) && oldToNewSourceMap.contains(dstUUID)) {
                    // 说明连接线两端的资源也同时会被粘贴创建
                    // 从原始连接线拷贝连接线上下文
                    PConnectorWireContext newConnectorCtx =
                            PConnectorWireContext(new ConnectorWireContext(*originalConnectorCtx.data()));
                    newConnectorCtx->setUUID(Utility::createUUID());
                    newConnectorCtx->setOriginalUUID(originalConnectorCtx->uuid());
                    // 连接线坐标点偏移
                    /*                     QPolygonF points = newConnectorCtx->points();
                                        for (int i = 0; i < points.size(); i++) {
                                            points[i] = points[i] + distanceOffset;
                                        }
                                        newConnectorCtx->setPoints(points); */
                    PSourceProxy newSrcSource = oldToNewSourceMap[srcUUID];
                    PSourceProxy newDstSource = oldToNewSourceMap[dstUUID];

                    newConnectorCtx->setSrcSource(newSrcSource);
                    newConnectorCtx->setDstSource(newDstSource);

                    dataPtr->newConnectorsMap[newConnectorCtx->uuid()] = newConnectorCtx;
                }
            }
        }
    }

    QList<GraphicsLayer *> copyLayers;
    // 创建资源图元
    QMapIterator<QString, PSourceProxy> newSourceIter(dataPtr->newSourcesMap);
    while (newSourceIter.hasNext()) {
        newSourceIter.next();
        PSourceProxy newSource = newSourceIter.value();
        newSource->setSourceCreateType(SourceProxy::kCreateCopy);
        dataPtr->canvasScene->loadFromSourceProxy(newSource);
        GraphicsLayer *layer = dataPtr->canvasScene->getTransformProxyGraphicsBySource(newSource);
        if (layer) {
            layer->addToGroup(dataPtr->tempGroup);
            // 数据埋点，统计模块使用次数
            BuriedData &data = BuriedData::getInstance();
            QString typeString = "Control";

            if (canvasCtx->type() == CanvasContext::kElectricalType
                || canvasCtx->type() == CanvasContext::kElecUserDefinedType) {
                typeString = "Electrical";
            }
            data.increaseSourceUsage(typeString.toStdString(), newSource->prototypeName().toStdString());
        }
    }

    // 创建连接线图元
    QMapIterator<QString, PConnectorWireContext> newConnectorIter(dataPtr->newConnectorsMap);
    while (newConnectorIter.hasNext()) {
        newConnectorIter.next();
        QString uuid = newConnectorIter.key();
        PConnectorWireContext newConnectorCtx = newConnectorIter.value();

        if (dataPtr->canvasScene->getConnectorWireGraphicsByID(uuid) != nullptr) {
            continue;
        }
        ConnectorWireGraphicsObject *wire = new ConnectorWireGraphicsObject(dataPtr->canvasScene);
        if (!wire) {
            continue;
        }
        wire->loadFromContext(newConnectorCtx);

        if (wire->isConnectedWithPort()) {
            // 连接成功，把连接线上下文添加到画板
            canvasCtx->addConnectWireContext(newConnectorCtx);

            wire->addToGroup(dataPtr->tempGroup);
        }
    }
    // dataPtr->canvasScene->refreshInputAndOutPutIndex();
    dataPtr->canvasScene->addItem(dataPtr->tempGroup);
    dataPtr->canvasScene->setTransformItemGroup(dataPtr->tempGroup);
    dataPtr->tempGroup->setSelected(true);
    dataPtr->tempGroup->setUnderMouseProxy();
    clearUnderMouseProxy();
}

void TransformItemGroup::addThroughAndDisableAction(QSharedPointer<QMenu> menu)
{
    if (!menu || !dataPtr->canvasScene || !dataPtr->canvasScene->getCanvasContext())
        return;
    // 电气画板不需要标记直通禁用等action,画板锁定等状态也不添加这些action
    if (dataPtr->canvasScene->getCanvasContext()->type() == CanvasContext::kElecUserDefinedType
        || dataPtr->canvasScene->getCanvasContext()->type() == CanvasContext::kElectricalType
        || !dataPtr->canvasScene->getCanvasContext()->canModify()) {
        return;
    }

    bool hasSource = false;
    bool hasThroughState = false;
    bool hasDisableState = false;
    bool hasNormalState = false;
    bool hasOtherState = false;
    for each (auto layer in dataPtr->layersMap) {
        if (!layer)
            continue;
        TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(layer);
        if (proxyGraphics
            && (proxyGraphics->getSourceGraphicsObject()->getLayerFlag() & GraphicsLayer::kSourceGraphicsLayer)) {
            QString protypeName = proxyGraphics->getSourceProxy()->prototypeName();
            if (protypeName == "In" || protypeName == "Out") // in out模块不设置注释
                continue;
            hasSource = true;
            QString state = proxyGraphics->getSourceProxy()->getCommentState();
            if (state == "through") {
                hasThroughState = true;
            } else if (state == "disable") {
                hasDisableState = true;
            } else if (state == "normal") {
                hasNormalState = true;
            } else {
                hasOtherState = true;
            }
        }
    }

    if (hasSource && !hasOtherState) {
        QAction *actionDelete = ActionManager::getInstance().getAction(ActionManager::Delete);
        QAction *actionThourh = ActionManager::getInstance().getAction(ActionManager::MarkThrough);
        QAction *actionDisable = ActionManager::getInstance().getAction(ActionManager::MarkDisable);
        QAction *actionUnmark = ActionManager::getInstance().getAction(ActionManager::Unmark);
        if (!actionDelete || !actionThourh || !actionDisable || !actionUnmark)
            return;
        int index = menu->actions().indexOf(actionDelete);
        menu->insertAction(menu->actions().at(index + 1), actionThourh);
        menu->insertAction(menu->actions().at(index + 2), actionDisable);
        menu->insertAction(menu->actions().at(index + 3), actionUnmark);
        if (hasThroughState && !hasDisableState && !hasNormalState) { // 全都是直通状态那标记直通的action就禁用掉
            actionThourh->setEnabled(false);
            actionDisable->setEnabled(true);
            actionUnmark->setEnabled(true);
        } else if (hasDisableState && !hasThroughState && !hasNormalState) { // 全都是禁用状态那标记禁用的action就禁用掉
            actionThourh->setEnabled(true);
            actionDisable->setEnabled(false);
            actionUnmark->setEnabled(true);
        } else {
            actionThourh->setEnabled(true);
            actionDisable->setEnabled(true);
            actionUnmark->setEnabled(true);
        }
    }
}

void TransformItemGroup::createAlignLine()
{
    if (dataPtr->proxyUnderMouse && !dataPtr->isCopying) {
        dataPtr->proxyUnderMouse->createAlignLine();
    }
}

void TransformItemGroup::setUnderMouseProxy()
{
    if (!dataPtr->canvasScene) {
        return;
    }
    auto canvasView = dataPtr->canvasScene->getCanvasView();
    if (!canvasView) {
        return;
    }
    for each (auto item in childItems()) {
        TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
        if (proxyGraphics) {
            QRectF rect = proxyGraphics->mapRectToScene(proxyGraphics->unitedChildsBoundingRect());
            QRectF viewRect = canvasView->mapFromScene(rect).boundingRect();
            QPointF viewPoint = canvasView->mapFromGlobal(QCursor::pos());
            if (viewRect.contains(viewPoint)) {
                dataPtr->proxyUnderMouse = proxyGraphics;
            }
        }
    }
}

void TransformItemGroup::clearUnderMouseProxy()
{
    dataPtr->proxyUnderMouse = nullptr;
}