#include "ConnectorWireGraphicsObject.h"
#include "BezierConnectorWireAlgorithm.h"
#include "CanvasContext.h"
#include "CanvasViewDefaultImpl.h"
#include "ConnectorWireCommand.h"
#include "ConnectorWireContext.h"
#include "ConnectorWireGraphicsObjectPrivate.h"
#include "ConnectorWireRegulationController.h"
#include "ConnectorWireSegment.h"
#include "ConnectorWireSwitch.h"
#include "GraphicsKernelDefinition.h"
#include "ICanvasScene.h"
#include "ManhattaConnectorWireAlgorithm.h"
#include "PortContext.h"
#include "PortGraphicsObject.h"
#include "PortPositionCommand.h"
#include "SignalNameGraphicsObject.h"
#include "SourceGraphicsObject.h"
#include "SourceProxy.h"
#include "StraightLineConnectorWireAlgorithm.h"
#include "TransformItemGroup.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"
#include "actionmanager.h"
#include "graphicsmodelingkernel.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QList>
#include <QPainter>
#include <QSet>
#include <qmath.h>

ConnectorWireGraphicsObject::ConnectorWireGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent)

{
    dataPtr.reset(new ConnectorWireGraphicsObjectPrivate());
    setAlogrithm(ManhattaAlgorithmName);
    setFlags(ItemIsSelectable | ItemSendsGeometryChanges);

    if (canvasScene) {
        auto canvasContext = canvasScene->getCanvasContext();
        if (canvasContext) {
            setAlogrithm(canvasContext->connectorWireAlgorithmName());
            connect(canvasContext.data(), SIGNAL(canvasContextChanged(QString, QVariant)), this,
                    SLOT(onConnectorWireAlgorithmChanged(QString, QVariant)));
        }
    }

    dataPtr->context = QSharedPointer<ConnectorWireContext>(new ConnectorWireContext(Utility::createUUID()));
    connect(dataPtr->context.data(), &ConnectorWireContext::pointsUpdated, this,
            &ConnectorWireGraphicsObject::onPointsUpdated);

    dataPtr->signalGraphicsObject = nullptr;

    int layerFlag = kConnectorWireGraphicsLayer | kSelectedGraphicsLayer;
    setLayerFlag((GraphicsLayer::GraphicsLayerFlag)layerFlag);

    dataPtr->outputController = new ConnectorWireRegulationController(this, PortOutputType);
    dataPtr->outputController->setVisible(false);
    connect(dataPtr->outputController, &ConnectorWireRegulationController::positionChanged, this,
            &ConnectorWireGraphicsObject::onRegulationControllerPositionChanged);
    dataPtr->inputController = new ConnectorWireRegulationController(this, PortInputType);
    dataPtr->inputController->setVisible(false);
    connect(dataPtr->inputController, &ConnectorWireRegulationController::positionChanged, this,
            &ConnectorWireGraphicsObject::onRegulationControllerPositionChanged);
}

ConnectorWireGraphicsObject::~ConnectorWireGraphicsObject()
{
    if (dataPtr->connectorWireSwitchGraphics) {
        dataPtr->connectorWireSwitchGraphics->detachConnectorWire();
        auto canvasScene = getCanvasScene();
        if (canvasScene) {
            dataPtr->connectorWireSwitchGraphics->cleanLayerInfo();
            canvasScene->removeItem(dataPtr->connectorWireSwitchGraphics);
        }
    }
    disconnect();
}

int ConnectorWireGraphicsObject::type() const
{
    return kConnectorWireGraphics;
}

QString ConnectorWireGraphicsObject::id()
{
    if (dataPtr->context) {
        return dataPtr->context->uuid();
    }
    return "";
}

void ConnectorWireGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext || !dataPtr->context) {
        return;
    }

    if (dataPtr->inputController && dataPtr->inputController->getMatchedRegulationController()) {
        return;
    }

    if (dataPtr->outputController && dataPtr->outputController->getMatchedRegulationController()) {
        return;
    }

    painter->save();

    QString algorithmName = getAlgorithmName();
    if (algorithmName == ManhattaAlgorithmName) {
        if (isConnectedWithPort()) {
            // 绘制分支点
            painter->setRenderHint(QPainter::Antialiasing, true);
            for each (QPointF p in dataPtr->branchPoints) {
                painter->setPen(QPen(QColor("#252d3f"), 1, Qt::SolidLine));
                painter->setBrush(QBrush(QColor("#252d3f"), Qt::SolidPattern));
                painter->drawEllipse(p, 2, 2);
            }

            // 绘制箭头
            if (dataPtr->arrowPoints.size() > 0) {
                QPen pen(0x2E9FE6);
                pen.setWidthF(2);
                painter->setPen(pen);
                painter->setBrush(Qt::NoBrush);
                painter->drawPolyline(mapFromScene(dataPtr->arrowPoints));
            }
        } else {
            if (canvasContext->isSupportSuspendedConnectorWire()) {
                painter->setPen(QPen(QColor(Qt::red), 1, Qt::DotLine));
            }

            painter->drawPolyline(dataPtr->points);
        }

    } else {
        painter->drawPolyline(dataPtr->points);
    }

    painter->restore();
}

void ConnectorWireGraphicsObject::cachePoints()
{
    if (!dataPtr->context) {
        return;
    }

    if (dataPtr->points.isEmpty()) {
        return;
    }

    dataPtr->context->setPoints(dataPtr->points);

    if (dataPtr->outPutPortGraphicsObject) {
        dataPtr->lastStartPos = dataPtr->outPutPortGraphicsObject->getLinkPos();
        dataPtr->lastStartAnchor = dataPtr->outPutPortGraphicsObject->getAnchorPoint();
        if (!dataPtr->outputController->isHover) {
            dataPtr->outputController->setVisible(false);
        }
    } else {
        dataPtr->outputController->setVisible(true);
        dataPtr->outputController->setPos(dataPtr->points.first());
    }
    if (dataPtr->inPutPortGraphicsObject) {
        dataPtr->lastEndPos = dataPtr->inPutPortGraphicsObject->getLinkPos();
        dataPtr->lastEndAnchor = dataPtr->inPutPortGraphicsObject->getAnchorPoint();
        if (!dataPtr->inputController->isHover) {
            dataPtr->inputController->setVisible(false);
        }
    } else {
        dataPtr->inputController->setVisible(true);
        dataPtr->inputController->setPos(dataPtr->points.last());
    }

    // 折线算法处理成线段组
    manhattanWireCachePoints();

    /// 调整连接线开关位置
    adjustWireSwitchPos();

    /// 调整连接线信号图元位置
    adjustWireSignalNamePos();
}

void ConnectorWireGraphicsObject::applyOffsetPos()
{
    QPointF nowPos = pos();
    if (nowPos != QPointF(0, 0)) {
        QPolygonF tmpPoints;
        for each (auto p in dataPtr->points) {
            QPointF newPos = p + nowPos;
            tmpPoints.append(newPos);
        }
        setPoints(tmpPoints);
        setPos(QPointF(0, 0));
    }
}

void ConnectorWireGraphicsObject::addBranchPoint(const QPointF &pt)
{
    dataPtr->branchPoints.append(pt);
}

PortGraphicsObject *ConnectorWireGraphicsObject::getOutputTypePortGraphics()
{
    return dataPtr->outPutPortGraphicsObject;
}

PortGraphicsObject *ConnectorWireGraphicsObject::getInputTypePortGraphics()
{
    return dataPtr->inPutPortGraphicsObject;
}

PortGraphicsObject *ConnectorWireGraphicsObject::getStartPortGraphics()
{
    return dataPtr->outPutPortGraphicsObject;
}

PortGraphicsObject *ConnectorWireGraphicsObject::getEndPortGraphics()
{
    return dataPtr->inPutPortGraphicsObject;
}

ConnectorWireSwitch *ConnectorWireGraphicsObject::getConnectorWireSwitch()
{
    return dataPtr->connectorWireSwitchGraphics;
}

bool ConnectorWireGraphicsObject::isSourceBothSelected()
{
    if (!dataPtr->outPutPortGraphicsObject || !dataPtr->inPutPortGraphicsObject) {
        return false;
    }
    auto startSourceProxyGraphics = dataPtr->outPutPortGraphicsObject->getTransformProxyGraphicsObject();
    auto endSourceProxyGraphics = dataPtr->inPutPortGraphicsObject->getTransformProxyGraphicsObject();
    if (startSourceProxyGraphics && endSourceProxyGraphics) {
        return startSourceProxyGraphics->isSelected() && endSourceProxyGraphics->isSelected();
    }

    return false;
}

void ConnectorWireGraphicsObject::enableHighLightHomologous(bool enable)
{
    dataPtr->highLightHomologous = enable;
}

bool ConnectorWireGraphicsObject::isHighLightHomologous()
{
    return dataPtr->highLightHomologous;
}

QRectF ConnectorWireGraphicsObject::boundingRect() const
{
    return shape().controlPointRect();
}

bool ConnectorWireGraphicsObject::linkPortGraphicsObject(PortGraphicsObject *portGraphics)
{
    if (!portGraphics) {
        return false;
    }

    if (!portGraphics->canLink()) // 端口不能链接
    {
        return false;
    }

    auto portProxyGraphics = portGraphics->getTransformProxyGraphicsObject();
    if (!portProxyGraphics) {
        return false;
    }

    auto portSourceProxy = portProxyGraphics->getSourceProxy();

    bool flag = true;
    QSharedPointer<PortContext> portContext = portGraphics->getPortContext();
    if (portContext) {
        QString linkPortType = portContext->type();
        if (dataPtr->oldPortContext && dataPtr->oldPortContext->type() != linkPortType) {
            return false;
        }

        if (linkPortType == PortInputType) {
            if (dataPtr->inPutPortGraphicsObject) {
                dataPtr->inPutPortGraphicsObject->unLinkConnectorWire(this);
            }
            dataPtr->inPutPortGraphicsObject = portGraphics;
            dataPtr->context->setDstSource(portSourceProxy);
            dataPtr->context->setDstPortID(portContext->uuid());
        } else if (linkPortType == PortOutputType) {
            if (dataPtr->outPutPortGraphicsObject) {
                dataPtr->outPutPortGraphicsObject->unLinkConnectorWire(this);
            }
            dataPtr->outPutPortGraphicsObject = portGraphics;
            dataPtr->context->setSrcSource(portSourceProxy);
            dataPtr->context->setSrcPortID(portContext->uuid());
        } else {
            return false;
        }

        // 端口和连接线关联
        portGraphics->linkConnectorWire(this);
        if (isConnectedWithPort()) {
            portGraphics->linkChanged();

            // 如果不是从内存中加载的连接线，触发连接状态变更
            // 电气传输线重新从画板加载时，不应该在计算自动布局了
            if (!dataPtr->isFromLoad) {
                dataPtr->outPutPortGraphicsObject->autoLayout();
                dataPtr->inPutPortGraphicsObject->autoLayout();
            }

            if (dataPtr->context->autoCalculation()) {
                dataPtr->points.clear();

                QPainterPath obstaclePath;
                auto canvasScene = getCanvasScene();
                if (canvasScene) {
                    obstaclePath = canvasScene->getTransformProxyGraphicsPainterPath();
                }

                dataPtr->points = dataPtr->algorithm->getAlgorithmPath(
                        dataPtr->outPutPortGraphicsObject->getLinkPos(),
                        dataPtr->outPutPortGraphicsObject->getExternalPos(),
                        dataPtr->inPutPortGraphicsObject->getLinkPos(),
                        dataPtr->inPutPortGraphicsObject->getExternalPos(), obstaclePath);

                dataPtr->shouldAvoidCollision = true;

            } else {
                dataPtr->points = dataPtr->context->points();
            }
            cachePoints();

            QSharedPointer<CanvasContext> canvasCtx = getCanvasContext();
            if (canvasCtx
                && (canvasCtx->type() == CanvasContext::kElectricalType
                    || canvasCtx->type() == CanvasContext::kElecUserDefinedType)) {
                // add by liwenyu 2023.06.12 电气画板，把母线或者点状母线的层级置于连接线之上 bugfix:
                // https://zt.xtkfpt.online/bug-view-8137.html
                auto endTransofrmProxy = dataPtr->inPutPortGraphicsObject->getTransformProxyGraphicsObject();
                if (endTransofrmProxy) {
                    endTransofrmProxy->setZValue(zValue() + 1);
                }
            }
        }
        return true;
    }
    return false;
}

bool ConnectorWireGraphicsObject::unlinkPortGraphicsObject(PortGraphicsObject *portGraphics)
{
    if (portGraphics == nullptr) {
        return false;
    }
    portGraphics->unLinkConnectorWire(this);
    if (portGraphics == dataPtr->outPutPortGraphicsObject) {
        if (dataPtr->outputController) {
            dataPtr->outputController->blockSignals(true);
            dataPtr->outputController->setPos(portGraphics->getLinkPos());
            dataPtr->outputController->blockSignals(false);
        }
        dataPtr->outPutPortGraphicsObject = nullptr;
    }
    if (portGraphics == dataPtr->inPutPortGraphicsObject) {
        if (dataPtr->inputController) {
            dataPtr->inputController->blockSignals(true);
            dataPtr->inputController->setPos(portGraphics->getLinkPos());
            dataPtr->inputController->blockSignals(false);
        }
        dataPtr->inPutPortGraphicsObject = nullptr;
    }

    if (!dataPtr->inPutPortGraphicsObject || !dataPtr->outPutPortGraphicsObject) {
        if (dataPtr->startSegment) {
            dataPtr->startSegment->setVisible(false);
        }
        if (dataPtr->endSegment) {
            dataPtr->endSegment->setVisible(false);
        }
        for each (auto segment in dataPtr->segments) {
            if (segment) {
                segment->setVisible(false);
            }
        }
    }
    return true;
}

void ConnectorWireGraphicsObject::clearLinks()
{
    if (dataPtr->outPutPortGraphicsObject) {
        dataPtr->outPutPortGraphicsObject->unLinkConnectorWire(this);
        dataPtr->outPutPortGraphicsObject = nullptr;
    }
    if (dataPtr->inPutPortGraphicsObject) {
        dataPtr->inPutPortGraphicsObject->unLinkConnectorWire(this);
        dataPtr->inPutPortGraphicsObject = nullptr;
    }
}

void ConnectorWireGraphicsObject::setMovingPos(QPointF pos)
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }
    QPainterPath obsclePath = canvasScene->getTransformProxyGraphicsPainterPath();
    dataPtr->movingPos = pos;
    if (nullptr == dataPtr->algorithm) {
        setAlogrithm(StraightLineAlgorithmName);
    }
    QRectF sceneRect = canvasScene->sceneRect();
    if (dataPtr->outPutPortGraphicsObject && dataPtr->inPutPortGraphicsObject) {
        dataPtr->points = dataPtr->algorithm->getAlgorithmPath(
                dataPtr->outPutPortGraphicsObject->getLinkPos(), dataPtr->outPutPortGraphicsObject->getExternalPos(),
                dataPtr->inPutPortGraphicsObject->getLinkPos(), dataPtr->inPutPortGraphicsObject->getExternalPos(),
                obsclePath);
    } else {
        if (dataPtr->outPutPortGraphicsObject) {
            dataPtr->points = dataPtr->algorithm->getAlgorithmPath(dataPtr->outPutPortGraphicsObject->getLinkPos(),
                                                                   dataPtr->outPutPortGraphicsObject->getExternalPos(),
                                                                   dataPtr->movingPos, dataPtr->movingPos, obsclePath);
        }
        if (dataPtr->inPutPortGraphicsObject) {
            dataPtr->points = dataPtr->algorithm->getAlgorithmPath(
                    dataPtr->movingPos, dataPtr->movingPos, dataPtr->inPutPortGraphicsObject->getLinkPos(),
                    dataPtr->inPutPortGraphicsObject->getExternalPos(), obsclePath);
        }
    }
    cachePoints();
}

bool ConnectorWireGraphicsObject::isConnectedWithPort()
{
    if (!dataPtr->outPutPortGraphicsObject || !dataPtr->inPutPortGraphicsObject) {
        return false;
    }
    // 连接线的两个端口类型不一致才算连接成功(输出->输入)
    return dataPtr->outPutPortGraphicsObject->isOutputPort() != dataPtr->inPutPortGraphicsObject->isOutputPort();
}

void ConnectorWireGraphicsObject::setAlogrithm(QString algoritmName)
{
    if (dataPtr->algorithm) {
        QString name = dataPtr->algorithm->getAlgorithmName();
        if (name == algoritmName) {
            return;
        }
        delete dataPtr->algorithm;
        dataPtr->algorithm = nullptr;
    }
    if (StraightLineAlgorithmName == algoritmName) {
        dataPtr->algorithm = new StraightLineConnectorWireAlgorithm();
    } else if (ManhattaAlgorithmName == algoritmName) {
        ManhattaConnectorWireAlgorithm *algorithm = new ManhattaConnectorWireAlgorithm();
        dataPtr->algorithm = algorithm;
    } else if (BezierAlogrithmName == algoritmName) {
        dataPtr->algorithm = new BezierConnectorWireAlgorithm();
    } else {
        ManhattaConnectorWireAlgorithm *algorithm = new ManhattaConnectorWireAlgorithm();
        dataPtr->algorithm = algorithm;
    }
    if (dataPtr->outPutPortGraphicsObject && dataPtr->inPutPortGraphicsObject) {
        dataPtr->points.clear();
        auto canvasScene = getCanvasScene();
        QPainterPath obstaclePath;
        if (canvasScene) {
            obstaclePath = canvasScene->getTransformProxyGraphicsPainterPath();
        }
        dataPtr->points = dataPtr->algorithm->getAlgorithmPath(
                dataPtr->outPutPortGraphicsObject->getLinkPos(), dataPtr->outPutPortGraphicsObject->getExternalPos(),
                dataPtr->inPutPortGraphicsObject->getLinkPos(), dataPtr->inPutPortGraphicsObject->getExternalPos(),
                obstaclePath);
        cachePoints();
    }
}

QString ConnectorWireGraphicsObject::getAlgorithmName()
{
    if (!dataPtr->algorithm) {
        return "";
    }
    return dataPtr->algorithm->getAlgorithmName();
}

bool ConnectorWireGraphicsObject::loadFromContext(QSharedPointer<ConnectorWireContext> ctx)
{

    ICanvasScene *canvasScene = getCanvasScene();
    if (ctx.isNull() || !canvasScene) {
        return false;
    }

    auto canvasContext = canvasScene->getCanvasContext();
    bool isSupportSuspendedConnectorWire = canvasContext->isSupportSuspendedConnectorWire();
    dataPtr->context = ctx;

    connect(ctx.data(), &ConnectorWireContext::pointsUpdated, this, &ConnectorWireGraphicsObject::onPointsUpdated);

    dataPtr->isFromLoad = true;

    PSourceProxy srcSource = ctx->srcSource();
    PSourceProxy dstSource = ctx->dstSource();
    PortGraphicsObject *srcPortGraphics = nullptr;
    PortGraphicsObject *dstPortGraphics = nullptr;
    if (isSupportSuspendedConnectorWire) {
        // 连接线两端没有连接任何端口，目前仍然不允许
        if (srcSource.isNull() && dstSource.isNull()) {
            return false;
        }
        if (srcSource) {
            srcPortGraphics = canvasScene->getPortGraphics(srcSource->uuid(), ctx->srcPortID());
        }
        if (dstSource) {
            dstPortGraphics = canvasScene->getPortGraphics(dstSource->uuid(), ctx->dstPortID());
        }
        if (!srcPortGraphics && !dstPortGraphics) {
            return false;
        }

        if (srcPortGraphics && dstPortGraphics) {
            // 如果两个端口类型一致，不允许连接
            if (srcPortGraphics->isOutputPort() == dstPortGraphics->isOutputPort()) {
                return false;
            }
        }
        canvasScene->addConnectorWireGraphicsObject(this, false);
        dataPtr->context->blockSignals(true);
        linkPortGraphicsObject(srcPortGraphics);
        linkPortGraphicsObject(dstPortGraphics);
        dataPtr->context->blockSignals(false);
        dataPtr->points = ctx->points();
        cachePoints();

        return true;

    } else {
        if (srcSource.isNull() || dstSource.isNull()) {
            return false;
        }
        srcPortGraphics = canvasScene->getPortGraphics(srcSource->uuid(), ctx->srcPortID());
        dstPortGraphics = canvasScene->getPortGraphics(dstSource->uuid(), ctx->dstPortID());
        if (!srcPortGraphics || !dstPortGraphics) {
            return false;
        }

        // 如果两个端口类型一致，不允许连接
        if (srcPortGraphics->isOutputPort() == dstPortGraphics->isOutputPort()) {
            return false;
        }

        if (ctx->points().isEmpty()) {
            // add by liwenyu 2023.07.05 此处把CanvasContext的信号屏蔽，否则当打开旧版本生成的画板时，
            // 此处读取的坐标点为空(32位和64位程序数据长度不一致导致)，会触发连接线上下文变更信息，导致外部标记为已修改，关闭窗口的时候会提示是否保存
            dataPtr->context->blockSignals(true);
            // 反序列化的结果不包含坐标点数据，(SimuCAD导入模块会出现这种情况)
            linkPortGraphicsObject(srcPortGraphics);
            linkPortGraphicsObject(dstPortGraphics);
            dataPtr->context->blockSignals(false);
            if (isConnectedWithPort()) {
                canvasScene->addConnectorWireGraphicsObject(this, false);
                return true;
            }
            return false;
        }

        if (srcPortGraphics->isOutputPort()) {
            dataPtr->outPutPortGraphicsObject = srcPortGraphics;
        } else {
            dataPtr->inPutPortGraphicsObject = srcPortGraphics;
        }

        if (dstPortGraphics->isOutputPort()) {
            dataPtr->outPutPortGraphicsObject = dstPortGraphics;
        } else {
            dataPtr->inPutPortGraphicsObject = dstPortGraphics;
        }
        dataPtr->points = ctx->points();

        if (isConnectedWithPort()) {
            canvasScene->addConnectorWireGraphicsObject(this, false);
            dataPtr->outPutPortGraphicsObject->linkConnectorWire(this);
            dataPtr->inPutPortGraphicsObject->linkConnectorWire(this);

            manhattanWireFixPatch();

            cachePoints();
            return true;
        }
    }

    return false;
}

QVariant ConnectorWireGraphicsObject::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case QGraphicsItem::ItemPositionChange:
        dataPtr->movingPos = value.toPointF();
        dataPtr->movingPos = Utility::pointAlignmentToGrid(dataPtr->movingPos, 10);
        return dataPtr->movingPos;
        break;
    case QGraphicsItem::ItemSelectedHasChanged: {
        bool isSelectedStatus = isSelected();

        if (!isSelectedStatus) {
            if (dataPtr->startSegment && dataPtr->endSegment && !dataPtr->segments.isEmpty()) {
                // 连接线取消选中时，线段也取消选中
                dataPtr->startSegment->blockSignals(true);
                dataPtr->endSegment->blockSignals(true);
                dataPtr->startSegment->setSelected(isSelectedStatus);
                dataPtr->endSegment->setSelected(isSelectedStatus);
                dataPtr->startSegment->blockSignals(false);
                dataPtr->endSegment->blockSignals(false);
                for each (auto segment in dataPtr->segments) {
                    if (segment) {
                        segment->blockSignals(true);
                        segment->setSelected(isSelectedStatus);
                        segment->blockSignals(false);
                    }
                }
            }
        }
        dataPtr->hasSegmentSelected = isSelectedStatus;
        syncHighLightHomologousStatus();

        CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(getCanvasScene());
        if (defaultScene) {
            defaultScene->processSelectionChanged();
        }

    } break;
    case QGraphicsItem::ItemPositionHasChanged: {
        dataPtr->context->setPos(pos());
    } break;
    case QGraphicsItem::ItemVisibleHasChanged: {

    } break;
    default: {
    }
    }
    return GraphicsLayer::itemChange(change, value);
}

QPainterPath ConnectorWireGraphicsObject::shape() const
{
    QPainterPath path;
    int pointsSize = dataPtr->points.size();

    if (pointsSize > 0) {
        for (int i = 0; i < pointsSize - 1; i++) {
            if (i == 0 && dataPtr->outPutPortGraphicsObject) {
                // 把连接线输出端口的检测区域调整一下，避免已经连接到连接线的输出端口，在点击端口边缘时，被判定点击到连接线，
                // 导致无法创建新的连接线
                path.moveTo(adjustShapePoint(dataPtr->outPutPortGraphicsObject->getAnchorPoint(), dataPtr->points.at(i),
                                             dataPtr->pathStrokerWidth));
            } else {
                path.moveTo(dataPtr->points.at(i));
            }
            path.lineTo(dataPtr->points.at(i + 1));
        }
    }

    QPainterPath retpath;
    QPen connectorPen;
    connectorPen.setWidthF(dataPtr->pathStrokerWidth);
    Utility::qt_graphicsItem_shapeFromPath(retpath, path, connectorPen);
    return retpath;
}

void ConnectorWireGraphicsObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    auto canvasCtx = getCanvasContext();
    if (!canvasCtx) {
        return;
    }
    bool canModify = canvasCtx->canModify();
    if (!dataPtr->signalGraphicsObject) {
        dataPtr->signalGraphicsObject = new SignalNameGraphicsObject(getCanvasScene(), this);
        connect(dataPtr->context.data(), &ConnectorWireContext::signalNameChanged, this,
                &ConnectorWireGraphicsObject::onSignalNameChanged);
        dataPtr->signalGraphicsObject->setPlainText("  ");
    }
    if (!dataPtr->signalGraphicsObject->isVisible()) {
        dataPtr->signalGraphicsObject->setPlainText("  ");
        dataPtr->signalGraphicsObject->userShow(true);
    }
    bool isVertical = false;
    QPointF signalPos = getSignalNamePos(isVertical);
    if (!isVertical) {
        QRectF rc = dataPtr->signalGraphicsObject->boundingRect();
        signalPos.setX(signalPos.x() - rc.width() / 2);
    }
    dataPtr->signalGraphicsObject->setPos(signalPos);
    dataPtr->signalGraphicsObject->textItem->setFocus();

    if (!canModify) {
        dataPtr->signalGraphicsObject->userShow(false);
    }

    GraphicsLayer::mouseDoubleClickEvent(event);
}

void ConnectorWireGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }
    // 鼠标点击连接线时，如果坐标位置存在代理图层，则忽略该点击事件
    // 解决点状母线不好选中的问题
    QPointF scenePos = event->scenePos();
    QList<QGraphicsItem *> items = canvasScene->items(scenePos);
    for each (auto item in items) {
        if (item->type() == kTransformProxyGraphics) {
            TransformProxyGraphicsObject *graphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
            if (graphics) {
                event->ignore();
                break;
            }
        }
    }
}

void ConnectorWireGraphicsObject::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    setSelected(true);
    QSharedPointer<QMenu> menu = QSharedPointer<QMenu>(new QMenu);
    menu->setMinimumWidth(150);
    QList<ActionManager::ActionType> types;
    types << ActionManager::Delete;
    menu->addActions(ActionManager::getInstance().getActionList(types));
    menu->setObjectName("uniformStyleMenu");
    menu->exec(event->screenPos());
    event->accept();
}

void ConnectorWireGraphicsObject::setAutoCalculation(bool flag)
{
    if (dataPtr->context) {
        bool oldValue = dataPtr->context->autoCalculation();
        dataPtr->context->setAutoCalculation(flag);
        if (flag) {
            auto canvasContext = getCanvasContext();
            if (canvasContext) {
                // 电气画板
                if (canvasContext->type() == CanvasContext::kElectricalType) {
                    // 电气画板的母线端口都是inPutPortGraphicsObject
                    if (dataPtr->inPutPortGraphicsObject && dataPtr->inPutPortGraphicsObject->getSourceGraphics()) {
                        PSourceProxy sourceProxy = dataPtr->inPutPortGraphicsObject->getSourceProxy();
                        // 重置母线端口的锚点方向
                        if (sourceProxy && sourceProxy->prototypeName() == "DotBusbar") {
                            dataPtr->inPutPortGraphicsObject->resetPortAnchor();
                        }
                    }
                }
            }
        }
    }
}

QPointF ConnectorWireGraphicsObject::adjustShapePoint(Qt::AnchorPoint anchor, QPointF pt, int offset)
{
    switch (anchor) {
    case Qt::AnchorLeft:
        pt.setX(pt.x() - offset);
        break;
    case Qt::AnchorRight:
        pt.setX(pt.x() + offset);
        break;
    case Qt::AnchorBottom:
        pt.setY(pt.y() + offset);
        break;
    case Qt::AnchorTop:
        pt.setY(pt.y() - offset);
        break;
    }
    return pt;
}

QSharedPointer<ConnectorWireContext> ConnectorWireGraphicsObject::getConnectorWireContext()
{
    return dataPtr->context;
}

bool ConnectorWireGraphicsObject::isAnchor()
{
    if (dataPtr->outPutPortGraphicsObject && dataPtr->inPutPortGraphicsObject) {
        if (dataPtr->lastStartPos == dataPtr->outPutPortGraphicsObject->getLinkPos()
            && dataPtr->lastEndPos == dataPtr->inPutPortGraphicsObject->getLinkPos()
            && dataPtr->lastStartAnchor == dataPtr->outPutPortGraphicsObject->getAnchorPoint()
            && dataPtr->lastEndAnchor == dataPtr->inPutPortGraphicsObject->getAnchorPoint()) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}
void ConnectorWireGraphicsObject::setArrowPoints(QPolygonF &points)
{
    dataPtr->arrowPoints = points;
}

QSharedPointer<WireParameters> ConnectorWireGraphicsObject::refreshWireParameters()
{
    if ((flags() & ItemIsMovable) && isAnchor()) {
        // 如果此时连接线的状态属于可以移动状态,不需要计算连接线，直接同步移动图元
        return QSharedPointer<WireParameters>();
    }

    if (!dataPtr->context) {
        return QSharedPointer<WireParameters>();
    }

    if (isConnectedWithPort()) {
        if (dataPtr->lastStartPos == dataPtr->outPutPortGraphicsObject->getLinkPos()
            && dataPtr->lastEndPos == dataPtr->inPutPortGraphicsObject->getLinkPos()
            && dataPtr->lastStartAnchor == dataPtr->outPutPortGraphicsObject->getAnchorPoint()
            && dataPtr->lastEndAnchor == dataPtr->inPutPortGraphicsObject->getAnchorPoint()
            && !dataPtr->context->autoCalculation()) {
            // 如果两个端口的坐标都没变化，不需要重新绘制连接线
            return QSharedPointer<WireParameters>();
            ;
        }

        // add by liwenyu 2023.06.21  针对点状母线的锚点方向处理，需要更新连接线检测碰撞区域
        auto srcSource = dataPtr->context->srcSource();
        auto dstSource = dataPtr->context->dstSource();
        if (srcSource && dstSource) {
            if (srcSource->prototypeName() == "DotBusbar" || dstSource->prototypeName() == "DotBusbar") {
                // 连接线连的是点状母线，次数刷新一下碰撞检测区域
                // 点状母线的锚点方向选取的是从不同方向计算的连接线中的最短路径，而且在相同路径下的时候，会根据连接线的直角数取最小值为最优解。
                // 如果此时不更新检测区域，那么在元件移动时，可能会触发碰撞区域，导致某一个方向的计算路径拐弯导致错误
                CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(getCanvasScene());
                if (defaultScene) {
                    defaultScene->refreshTransformProxyGraphicsPainterPath();
                }
            }
        }
        // end 2023.06.21

        auto wireParameterPtr = QSharedPointer<WireParameters>(new WireParameters);
        wireParameterPtr->wireUUID = id();
        wireParameterPtr->startPos = dataPtr->outPutPortGraphicsObject->getLinkPos();
        wireParameterPtr->endPos = dataPtr->inPutPortGraphicsObject->getLinkPos();
        wireParameterPtr->startExternalPos = dataPtr->outPutPortGraphicsObject->getExternalPos();
        wireParameterPtr->endExternalPos = dataPtr->inPutPortGraphicsObject->getExternalPos();
        wireParameterPtr->startAnchor = dataPtr->outPutPortGraphicsObject->getAnchorPoint();
        wireParameterPtr->endAnchor = dataPtr->inPutPortGraphicsObject->getAnchorPoint();
        wireParameterPtr->pathResult = dataPtr->points;
        wireParameterPtr->algorithmName = getAlgorithmName();
        // 如果连接线的两个端口坐标都变化了，或者连接线没有经过调节器调节属于自动计算连接线坐标点状态，都需要重新计算
        if ((dataPtr->lastStartPos != dataPtr->outPutPortGraphicsObject->getLinkPos()
             && dataPtr->lastEndPos != dataPtr->inPutPortGraphicsObject->getLinkPos())
            || dataPtr->context->autoCalculation()) {
            setAutoCalculation(true);
            wireParameterPtr->autoCalculate = true;

        } else {
            if (dataPtr->lastStartAnchor != dataPtr->outPutPortGraphicsObject->getAnchorPoint()
                || dataPtr->lastEndAnchor != dataPtr->inPutPortGraphicsObject->getAnchorPoint()) {
                setAutoCalculation(true);
                wireParameterPtr->autoCalculate = true;

            } else {
                setAutoCalculation(false);
                wireParameterPtr->autoCalculate = false;
                // 说明只有一个端口坐标发生变化,确定移动中的端口
                PortGraphicsObject *movingPortGraphics =
                        dataPtr->lastStartPos != dataPtr->outPutPortGraphicsObject->getLinkPos()
                        ? dataPtr->outPutPortGraphicsObject
                        : dataPtr->inPutPortGraphicsObject;
                if (movingPortGraphics == dataPtr->outPutPortGraphicsObject) {
                    wireParameterPtr->isStartMoving = true;
                    wireParameterPtr->isEndMoving = false;
                } else {
                    wireParameterPtr->isStartMoving = false;
                    wireParameterPtr->isEndMoving = true;
                }
            }
        }
        return wireParameterPtr;
    } else {
    }

    return QSharedPointer<WireParameters>();
}

void ConnectorWireGraphicsObject::doSwitch(bool value)
{
    if (dataPtr->connectorWireSwitchGraphics) {
        dataPtr->connectorWireSwitchGraphics->doSwitch(value);
    }
}

PortGraphicsObject *ConnectorWireGraphicsObject::getAnotherPort(PortGraphicsObject *port)
{
    if (nullptr == port) {
        return nullptr;
    }
    if (dataPtr->outPutPortGraphicsObject == port) {
        return dataPtr->inPutPortGraphicsObject;
    }
    if (dataPtr->inPutPortGraphicsObject == port) {
        return dataPtr->outPutPortGraphicsObject;
    }
    return nullptr;
}

QPolygonF ConnectorWireGraphicsObject::getPoints()
{
    return dataPtr->points;
}

void ConnectorWireGraphicsObject::setPoints(const QPolygonF &points)
{
    dataPtr->points = points;
    cachePoints();
}

void ConnectorWireGraphicsObject::cleanBranchPoints()
{
    dataPtr->branchPoints.clear();
    dataPtr->branchConnectorWireUUIDList.clear();
}

void ConnectorWireGraphicsObject::onConnectorWireAlgorithmChanged(QString key, QVariant value)
{
    if (GKD::CANVAS_WIRE_ALG != key) {
        return;
    }
    cleanBranchPoints();
    QString algoritmName = value.toString();
    bool isSegmentShow = false;
    if (algoritmName == ManhattaAlgorithmName) {
        isSegmentShow = true;
    }

    if (dataPtr->startSegment) {
        dataPtr->startSegment->setVisible(isSegmentShow);
    }
    if (dataPtr->endSegment) {
        dataPtr->endSegment->setVisible(isSegmentShow);
    }
    for each (auto segment in dataPtr->segments) {
        if (segment) {
            segment->setVisible(isSegmentShow);
        }
    }
    setAlogrithm(algoritmName);
}

void ConnectorWireGraphicsObject::onSwitchStatusChanged(bool isOn)
{
    dataPtr->context->setConnected(isOn);
    auto canvasContext = getCanvasContext();
    if (canvasContext) {
        BusinessHooksServer *businessHook = canvasContext->getBusinessHooksServer();
        if (!businessHook) {
            return;
        }
        businessHook->verifyCanvasStatus();
    }
}

void ConnectorWireGraphicsObject::onSegmentPositionChanged(QPointF pt)
{
    QString algorithmName = getAlgorithmName();
    if (algorithmName != ManhattaAlgorithmName) {
        return;
    }
    if (dataPtr->points.size() < 4) {
        return;
    }
    ConnectorWireSegment *senderSegment = qobject_cast<ConnectorWireSegment *>(sender());
    if (!senderSegment) {
        return;
    }
    if (senderSegment && senderSegment->isVisible()) {

        PortGraphicsObject *outputPort = getOutputTypePortGraphics();
        if (outputPort) {
            outputPort->cleanConnectorWireBranchPoints();
        }

        if (senderSegment->getSegmentType() != ConnectorWireSegment::kSegmentCommon) {
            processStartOrEndLineSegment(senderSegment);
        } else {
            int index = indexOfSegment(senderSegment);
            if (index == -1) {
                return;
            }
            // 需要加上开始的连接线线段
            index += 1;
            QPointF segmentPos = senderSegment->pos();
            QPoint startPos = senderSegment->getStartPoint();
            QPoint endPos = senderSegment->getEndPoint();
            if (dataPtr->segments.first() == senderSegment || dataPtr->segments.last() == senderSegment) {
                if (dataPtr->points.size() == 4) {
                    // 这种情况属于连接线整体是一根直线，而且只有四个坐标点，需要插入两个点，两个线段
                    ConnectorWireSegment *newSegment1 = new ConnectorWireSegment(getCanvasScene(), this);
                    dataPtr->segments.push_front(newSegment1);

                    ConnectorWireSegment *newSegment2 = new ConnectorWireSegment(getCanvasScene(), this);
                    dataPtr->segments.push_front(newSegment2);

                    QPointF basePt1 = dataPtr->points[1];
                    QPointF basePt2 = dataPtr->points[2];
                    if (!senderSegment->isVertical()) {
                        basePt1.setX(startPos.x() + segmentPos.x());
                        basePt2.setX(startPos.x() + segmentPos.x());
                    } else {
                        basePt1.setY(startPos.y() + segmentPos.y());
                        basePt2.setY(startPos.y() + segmentPos.y());
                    }

                    // 插入顺序不要更改
                    dataPtr->points.insert(2, basePt2);
                    dataPtr->points.insert(2, basePt1);

                } else {
                    ConnectorWireSegment *newSegment = new ConnectorWireSegment(getCanvasScene(), this);
                    if (index == 1) {
                        dataPtr->segments.push_front(newSegment);
                    } else {
                        dataPtr->segments.push_back(newSegment);
                    }

                    if (index == 1) {
                        QPoint basePt = dataPtr->points[index].toPoint();
                        if (senderSegment->isVertical()) {
                            dataPtr->points[index + 1].setX(startPos.x() + segmentPos.x());
                            basePt.setX(startPos.x() + segmentPos.x());
                        } else {
                            dataPtr->points[index + 1].setY(startPos.y() + segmentPos.y());
                            basePt.setY(startPos.y() + segmentPos.y());
                        }
                        dataPtr->points.insert(index + 1, basePt);

                    } else {
                        QPoint basePt = dataPtr->points[index + 1].toPoint();
                        if (senderSegment->isVertical()) {
                            dataPtr->points[index].setX(startPos.x() + segmentPos.x());
                            basePt.setX(startPos.x() + segmentPos.x());
                        } else {
                            dataPtr->points[index].setY(startPos.y() + segmentPos.y());
                            basePt.setY(startPos.y() + segmentPos.y());
                        }
                        dataPtr->points.insert(index + 1, basePt);
                    }
                }

            } else {
                if (senderSegment->isVertical()) {
                    dataPtr->points[index].setX(startPos.x() + segmentPos.x());
                    dataPtr->points[index + 1].setX(startPos.x() + segmentPos.x());

                } else {
                    dataPtr->points[index].setY(startPos.y() + segmentPos.y());
                    dataPtr->points[index + 1].setY(startPos.y() + segmentPos.y());
                }
            }
        }

        setAutoCalculation(false);

        cachePoints();
        if (outputPort) {
            outputPort->calcuteConnectorWireBranchPoints();
        }
    }
}

void ConnectorWireGraphicsObject::onSegmentSelectedChanged(bool selected)
{
    QString algorithmName = getAlgorithmName();
    if (algorithmName != ManhattaAlgorithmName) {
        return;
    }
    bool flag = false;
    if (selected) {
        flag = true;
    } else {
        bool segmentSelected = dataPtr->startSegment->isSelected() || dataPtr->endSegment->isSelected();
        if (segmentSelected) {
            flag = true;
        } else {
            for each (auto segment in dataPtr->segments) {
                if (segment) {
                    if (segment->isSelected()) {
                        flag = true;
                        break;
                    }
                }
            }
        }
    }

    // 线段选中状态发生变化
    if (dataPtr->hasSegmentSelected != flag) {
        dataPtr->hasSegmentSelected = flag;
        setSelected(dataPtr->hasSegmentSelected);
    }
}

void ConnectorWireGraphicsObject::onTransformProxyChanged()
{
    auto scene = getCanvasScene();
    if (!scene) {
        return;
    }
    bool isLoading = scene->isLoadingCanvas();
    // 从内存加载的连接线不进行重叠检查
    if (!isLoading && shouldAvoidCollision()) {

        QPainterPath path = scene->getTransformProxyGraphicsPainterPath();
        QDateTime start_time = QDateTime::currentDateTime();
        // 调整的时候会涉及到动态插入线段，所以用临时变量保存调整之前的线段列表
        auto tempSegments = dataPtr->segments;
        int size = tempSegments.size();
        for (int i = 0; i < size; i++) {

            ConnectorWireSegment *segment = tempSegments[i];
            bool isColliding = segment->collidesWithPath(path) || segment->isOverLapWithOtherSegment();
            if (!isColliding) {
                continue;
            }
            // 参考的线段
            ConnectorWireSegment *referenceSegment = nullptr;
            ConnectorWireSegment *limitSegment = nullptr;
            QPointF lastPos = segment->pos();

            ConnectorWireSegment::SegmentDirection referenceDirection = ConnectorWireSegment::kNone;
            if (0 == i) {
                referenceSegment = dataPtr->startSegment;
                referenceDirection = referenceSegment->getSegmentDirection();
            } else if (i == size - 1) {
                referenceSegment = dataPtr->endSegment;
                auto direction = referenceSegment->getSegmentDirection();
                switch (direction) {
                case ConnectorWireSegment::kLeftToRight:
                    referenceDirection = ConnectorWireSegment::kRightToLeft;
                    break;
                case ConnectorWireSegment::kTopToBottom:
                    referenceDirection = ConnectorWireSegment::kBottomToTop;
                    break;
                case ConnectorWireSegment::kRightToLeft:
                    referenceDirection = ConnectorWireSegment::kLeftToRight;
                    break;
                case ConnectorWireSegment::kBottomToTop:
                    referenceDirection = ConnectorWireSegment::kTopToBottom;
                    break;
                default:
                    break;
                }
            } else {
                referenceSegment = tempSegments[i - 1];
                referenceDirection = referenceSegment->getSegmentDirection();
            }

            // 禁用当前线段的信号功能
            segment->blockSignals(true);
            // 和当前线段有碰撞的图元

            int step = 1;
            QPointF adjustPos = lastPos;
            // 最多偏移20个网格
            int limitStep = 20;
            while (isColliding && step < limitStep) {
                int offset = (step++) * 10;
                switch (referenceDirection) {
                case ConnectorWireSegment::kLeftToRight:
                    adjustPos = QPointF(lastPos.x() + offset, lastPos.y());
                    break;
                case ConnectorWireSegment::kTopToBottom:
                    adjustPos = QPointF(lastPos.x(), lastPos.y() + offset);
                    break;
                case ConnectorWireSegment::kRightToLeft:
                    adjustPos = QPointF(lastPos.x() - offset, lastPos.y());
                    break;
                case ConnectorWireSegment::kBottomToTop:
                    adjustPos = QPointF(lastPos.x(), lastPos.y() - offset);
                    break;

                default:
                    break;
                }

                segment->setPos(adjustPos);
                isColliding = segment->isOverLapWithOtherSegment();
                // isColliding = segment->collidesWithPath(path) || segment->isOverLapWithOtherSegment();
            }
            // 仍然还是会碰撞
            if (isColliding) {
                adjustPos = lastPos;
            }
            segment->setPos(lastPos);
            segment->blockSignals(false);
            // 调整位置的时候，会触发信号，增加新的线段
            if (lastPos != adjustPos) {
                segment->setPos(adjustPos);
                setAutoCalculation(false);
            }
        }
    }
    dataPtr->shouldAvoidCollision = false;
}

void ConnectorWireGraphicsObject::onSignalNameChanged(QString name)
{
    QString algorithmName = getAlgorithmName();
    if (algorithmName != ManhattaAlgorithmName) {
        return;
    }
    if (dataPtr->signalGraphicsObject) {
        dataPtr->signalGraphicsObject->setPlainText(name);
        if (name.isEmpty()) {
            dataPtr->signalGraphicsObject->userShow(false);
        } else {
            bool isVertical = false;
            QPointF signalPos = getSignalNamePos(isVertical);
            if (!isVertical) {
                QRectF rc = dataPtr->signalGraphicsObject->boundingRect();
                signalPos.setX(signalPos.x() - rc.width() / 2);
            }
            dataPtr->signalGraphicsObject->setPos(signalPos);
        }
    }
}

void ConnectorWireGraphicsObject::onPointsUpdated(QPolygonF pts)
{
    dataPtr->points = pts;
    cachePoints();
}

void ConnectorWireGraphicsObject::onRegulationControllerPositionChanged(const QPointF &position)
{
    QPainterPath obscalePath;
    auto canvasScene = getCanvasScene();
    if (canvasScene) {
        obscalePath = canvasScene->getTransformProxyGraphicsPainterPath();
    }

    ConnectorWireRegulationController *controller = qobject_cast<ConnectorWireRegulationController *>(sender());
    if (controller == dataPtr->inputController) {
        if (dataPtr->inPutPortGraphicsObject) {
            unlinkPortGraphicsObject(dataPtr->inPutPortGraphicsObject);
        }

    } else if (controller == dataPtr->outputController) {
        if (dataPtr->outPutPortGraphicsObject) {
            unlinkPortGraphicsObject(dataPtr->outPutPortGraphicsObject);
        }
    }
    QPointF outputLinkPos = dataPtr->outPutPortGraphicsObject ? dataPtr->outPutPortGraphicsObject->getLinkPos()
                                                              : dataPtr->outputController->pos();
    QPointF outputExternalPos = dataPtr->outPutPortGraphicsObject ? dataPtr->outPutPortGraphicsObject->getExternalPos()
                                                                  : dataPtr->outputController->pos();
    QPointF inputLinkPos = dataPtr->inPutPortGraphicsObject ? dataPtr->inPutPortGraphicsObject->getLinkPos()
                                                            : dataPtr->inputController->pos();
    QPointF inputExternalPos = dataPtr->inPutPortGraphicsObject ? dataPtr->inPutPortGraphicsObject->getExternalPos()
                                                                : dataPtr->inputController->pos();

    dataPtr->points = dataPtr->algorithm->getAlgorithmPath(outputLinkPos, outputExternalPos, inputLinkPos,
                                                           inputExternalPos, obscalePath);
    cachePoints();
}

int ConnectorWireGraphicsObject::indexOfSegment(ConnectorWireSegment *segment)
{
    if (!segment) {
        return -1;
    }
    return dataPtr->segments.indexOf(segment);
}

bool ConnectorWireGraphicsObject::hasSegmentSelected()
{
    return dataPtr->hasSegmentSelected;
}

void ConnectorWireGraphicsObject::processStartOrEndLineSegment(ConnectorWireSegment *segment)
{
    if (!segment) {
        return;
    }
    PortGraphicsObject *adjustPortGraphics = segment->getSegmentType() == ConnectorWireSegment::kSegmentFirst
            ? dataPtr->outPutPortGraphicsObject
            : dataPtr->inPutPortGraphicsObject;
    if (!adjustPortGraphics) {
        return;
    }

    auto adjustPortContext = adjustPortGraphics->getPortContext();
    auto adjustSourceGraphics = adjustPortGraphics->getSourceGraphics();
    if (!adjustPortContext || !adjustSourceGraphics) {
        return;
    }

    QPointF segmentPos = segment->pos();
    QPoint segmentEndPos = segment->getEndPoint();
    qreal sceneYPos = segment->getStartPoint().y() + segmentPos.y();
    qreal sceneXPos = segment->getStartPoint().x() + segmentPos.x();
    QRectF proxySceneRect;
    auto adjustProxyGraphics = adjustPortGraphics->getTransformProxyGraphicsObject();
    if (adjustProxyGraphics) {
        proxySceneRect = adjustProxyGraphics->getTransformSceneRect();
    }

    QPointF portScenePos = adjustPortGraphics->getCenterPos();
    bool isLine = false;
    bool segmentIsVertical = segment->isVertical();
    // 限制移动范围
    if (!segmentIsVertical) {
        if (sceneYPos < proxySceneRect.top()) {
            sceneYPos = proxySceneRect.top();
        } else if (sceneYPos > proxySceneRect.bottom()) {
            sceneYPos = proxySceneRect.bottom();
        }
        portScenePos.setY(sceneYPos);
    } else {
        if (sceneXPos < proxySceneRect.left()) {
            sceneXPos = proxySceneRect.left();
        } else if (sceneXPos > proxySceneRect.right()) {
            sceneXPos = proxySceneRect.right();
        }
        portScenePos.setX(sceneXPos);
    }

    QPointF sourcePos = adjustSourceGraphics->mapFromScene(portScenePos);
    // 改变端口位置
    if (adjustPortContext->isAbsolutePos()) {
        adjustPortContext->setPos(sourcePos);
    } else {
        QRectF sourceBoundingRect = adjustSourceGraphics->boundingRect();
        QPointF adjustPos = adjustPortContext->pos();
        qreal xPercent = (sourcePos.x() - sourceBoundingRect.left()) * 1.0 / sourceBoundingRect.width();
        adjustPos.setX(xPercent);
        qreal yPencent = (sourcePos.y() - sourceBoundingRect.top()) * 1.0 / sourceBoundingRect.height();
        adjustPos.setY(yPencent);
        adjustPortContext->setPos(adjustPos);
    }
    adjustPortGraphics->updatePosition();

    // 端口位置发生变化，重新刷新连接线
    auto wires = adjustPortGraphics->getLinkedConnectorWireList();
    QList<QSharedPointer<WireParameters>> wireParametersList;
    for each (auto wire in wires) {
        if (wire) {
            auto paramaters = wire->refreshWireParameters();
            wireParametersList.push_back(paramaters);
        }
    }
    if (!wireParametersList.isEmpty()) {
        auto canvasScene = getCanvasScene();
        if (canvasScene) {
            canvasScene->refreshConnectorWires(wireParametersList);
        }
    }
}

void ConnectorWireGraphicsObject::syncHighLightHomologousStatus()
{
    QString algorithmName = getAlgorithmName();
    if (algorithmName != ManhattaAlgorithmName) {
        return;
    }
    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return;
    }
    bool highlightHomologousConnectorWire = canvasContext->highlightHomologousConnectorWire();
    if (!highlightHomologousConnectorWire) {
        return;
    }
    PortGraphicsObject *outputPort = getOutputTypePortGraphics();
    if (outputPort) {
        if (dataPtr->hasSegmentSelected) {
            auto wires = outputPort->getLinkedConnectorWireList();
            for each (auto wire in wires) {
                wire->enableHighLightHomologous(true);
            }
        } else {
            bool hasAnotherSelected = false;
            auto wires = outputPort->getLinkedConnectorWireList();
            // 确定输出端口的其他连接线有没有线段被选中
            for each (auto wire in wires) {
                if (wire && wire != this && wire->hasSegmentSelected()) {
                    hasAnotherSelected = true;
                    break;
                }
            }
            // 如果其他连接线也没有线段被选中，则把所有连接线的高亮同源连接线的状态设置为false
            if (!hasAnotherSelected) {
                for each (auto wire in wires) {
                    wire->enableHighLightHomologous(false);
                }
            }
        }
    }
}

bool ConnectorWireGraphicsObject::isPointsMatchManhattaAlgorithm()
{
    QString algorithmName = getAlgorithmName();
    if (algorithmName != ManhattaAlgorithmName) {
        return true;
    }
    int pointsCount = dataPtr->points.size();
    if (pointsCount < 4) {
        return false;
    }
    for (int i = 0; i < pointsCount - 1; i++) {
        QPoint pt1 = dataPtr->points[i].toPoint();
        QPoint pt2 = dataPtr->points[i + 1].toPoint();
        if (pt1.x() != pt2.x() && pt1.y() != pt2.y()) {
            // 说明当前线段既不是垂直也不是水平线段，不符合连接线算法
            return false;
        }
    }
    return true;
}

QPointF ConnectorWireGraphicsObject::getSignalNamePos(bool &isVertical)
{
    QString algorithmName = getAlgorithmName();
    if (algorithmName != ManhattaAlgorithmName) {
        if (dataPtr->points.isEmpty()) {
            return QPointF();
        }
        return dataPtr->points.first();
    }
    ConnectorWireSegment *maxLengthSegment = nullptr;
    int maxLength = 0;
    foreach (auto segment, dataPtr->segments) {
        int segmentDistance = segment->getDistance();
        if (segmentDistance > maxLength) {
            maxLengthSegment = segment;
            maxLength = segmentDistance;
        }
    }
    if (!maxLengthSegment) {
        if (dataPtr->points.isEmpty()) {
            return QPointF();
        } else {
            return dataPtr->points.first();
        }
    }
    qreal xPos = (maxLengthSegment->getStartPoint().x() + maxLengthSegment->getEndPoint().x()) / 2;
    qreal yPos = (maxLengthSegment->getStartPoint().y() + maxLengthSegment->getEndPoint().y()) / 2;
    isVertical = maxLengthSegment->isVertical();
    return QPointF(xPos, yPos);
}

bool ConnectorWireGraphicsObject::shouldAvoidCollision()
{
    // 只有是折线算法时才进行避障计算
    if (dataPtr->algorithm && dataPtr->algorithm->getAlgorithmName() == ManhattaAlgorithmName) {
        return dataPtr->shouldAvoidCollision;
    }
    return false;
}

void ConnectorWireGraphicsObject::manhattanWireFixPatch()
{
    if (!dataPtr->algorithm) {
        return;
    }
    if (dataPtr->algorithm->getAlgorithmName() != ManhattaAlgorithmName) {
        return;
    }
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }
    // 2023.04.10 by liwenyu 打补丁，控制系统端口改成simulink的连接方式，之前的画板数据的起点就不一致了
    // (以及升级到Qt5之后，Qt4的画板序列化的数据在Qt5中读取出来不正确)同步一下
    auto canvasCtx = canvasScene->getCanvasContext();
    if (dataPtr->points.first() != dataPtr->outPutPortGraphicsObject->getLinkPos()
        || dataPtr->points.last() != dataPtr->inPutPortGraphicsObject->getLinkPos()) {
        QPointF startOffset = dataPtr->points.first() - dataPtr->outPutPortGraphicsObject->getLinkPos();
        QPointF endOffset = dataPtr->points.last() - dataPtr->inPutPortGraphicsObject->getLinkPos();
        if (startOffset.manhattanLength() > 10 || endOffset.manhattanLength() > 10) {
            dataPtr->context->blockSignals(true);
            setAutoCalculation(true);
            dataPtr->context->blockSignals(false);
            auto param = refreshWireParameters();
            if (param) {
                QList<QSharedPointer<WireParameters>> parameters;
                parameters.push_back(param);
                canvasScene->refreshConnectorWires(parameters);
            }
        } else {
            dataPtr->points.first() = dataPtr->outPutPortGraphicsObject->getLinkPos();
            dataPtr->points.last() = dataPtr->inPutPortGraphicsObject->getLinkPos();
        }
    }
    // end 2023.04.10

    /// add by liwenyu 2023.06.20 避免数据保存异常的时候，重新加载时连接线由斜线的情况
    bool isMatchAlgorithm = isPointsMatchManhattaAlgorithm();
    if (!isMatchAlgorithm) {
        setAutoCalculation(true);
        auto param = refreshWireParameters();
        if (param) {
            QList<QSharedPointer<WireParameters>> parameters;
            parameters.push_back(param);
            canvasScene->refreshConnectorWires(parameters);
        }
    }
    // end

    if (dataPtr->context->signalName() != "") {
        if (!dataPtr->signalGraphicsObject) {
            dataPtr->signalGraphicsObject = new SignalNameGraphicsObject(getCanvasScene(), this);
            connect(dataPtr->context.data(), &ConnectorWireContext::signalNameChanged, this,
                    &ConnectorWireGraphicsObject::onSignalNameChanged);
            bool isVertical = false;
            QPointF signalPos = getSignalNamePos(isVertical);
            if (!isVertical) {
                QRectF rc = dataPtr->signalGraphicsObject->boundingRect();
                signalPos.setX(signalPos.x() - rc.width() / 2);
            }
        }
        dataPtr->signalGraphicsObject->setPlainText(dataPtr->context->signalName());
    }
}

void ConnectorWireGraphicsObject::manhattanWireCachePoints()
{
    if (!isConnectedWithPort()) {
        return;
    }
    QString algorithmName = getAlgorithmName();
    if (algorithmName != ManhattaAlgorithmName) {
        return;
    }
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }
    if (!dataPtr->startSegment) {
        dataPtr->startSegment = new ConnectorWireSegment(canvasScene, this);
        dataPtr->startSegment->setSegmentType(ConnectorWireSegment::kSegmentFirst);
    }
    if (!dataPtr->endSegment) {
        dataPtr->endSegment = new ConnectorWireSegment(canvasScene, this);
        dataPtr->endSegment->setSegmentType(ConnectorWireSegment::kSegmentEnd);
    }

    // 去掉开始和结束的连接线线段，计算还需要的连接线线段数量
    int segmentCounts = dataPtr->points.size() - 3;
    int existedSegmentCounts = dataPtr->segments.size();
    if (existedSegmentCounts < segmentCounts) {
        for (int i = 0; i < segmentCounts - existedSegmentCounts; i++) {
            ConnectorWireSegment *segment = new ConnectorWireSegment(canvasScene, this);
            dataPtr->segments.push_back(segment);
        }
    }

    // 不需要的线段图元
    QList<ConnectorWireSegment *> needlessSegments;
    if (existedSegmentCounts > segmentCounts) {
        for (int i = segmentCounts; i < existedSegmentCounts; i++) {
            ConnectorWireSegment *segment = dataPtr->segments[i];
            needlessSegments.push_back(segment);
        }
    }

    // 删除多于的线段图元
    for each (auto segment in needlessSegments) {
        if (segment) {
            dataPtr->segments.removeOne(segment);
            auto groupItem = group();
            if (groupItem) {
                groupItem->removeFromGroup(segment);
            }
            segment->setSelected(false);
            segment->setVisible(false);
            delete segment;
        }
    }
    if (dataPtr->startSegment) {
        dataPtr->startSegment->blockSignals(true);
        QPoint startPt = dataPtr->points[0].toPoint();
        QPoint endPt = dataPtr->points[1].toPoint();
        QPointF segmentPos = dataPtr->startSegment->pos();
        dataPtr->startSegment->setPoints(startPt - segmentPos.toPoint(), endPt - segmentPos.toPoint());
        dataPtr->startSegment->blockSignals(false);
        dataPtr->startSegment->setVisible(true);
    }
    if (dataPtr->endSegment) {
        dataPtr->endSegment->blockSignals(true);
        int pointsSize = dataPtr->points.size();
        QPoint startPt = dataPtr->points[pointsSize - 2].toPoint();
        QPoint endPt = dataPtr->points[pointsSize - 1].toPoint();
        QPointF segmentPos = dataPtr->endSegment->pos();
        dataPtr->endSegment->setPoints(startPt - segmentPos.toPoint(), endPt - segmentPos.toPoint());
        dataPtr->endSegment->blockSignals(false);
        dataPtr->endSegment->setVisible(true);
    }

    for (int i = 1; i < dataPtr->points.size() - 2; i++) {
        QPoint startPt = dataPtr->points[i].toPoint();
        QPoint endPt = dataPtr->points[i + 1].toPoint();
        dataPtr->segments[i - 1]->blockSignals(true);
        QPointF segmentPos = dataPtr->segments[i - 1]->pos();
        dataPtr->segments[i - 1]->setPoints(startPt - segmentPos.toPoint(), endPt - segmentPos.toPoint());
        dataPtr->segments[i - 1]->blockSignals(false);
        dataPtr->segments[i - 1]->setVisible(true);
    }
}

void ConnectorWireGraphicsObject::adjustWireSwitchPos()
{
    auto canvasScene = getCanvasScene();
    auto canvasContext = getCanvasContext();
    QString algorithmName = getAlgorithmName();
    if (algorithmName != ManhattaAlgorithmName || !isConnectedWithPort() || !canvasScene || !canvasContext) {
        return;
    }
    bool showSwitch = canvasContext->showConnectorWireSwitch();
    if (showSwitch) {
        if (dataPtr->connectorWireSwitchGraphics == nullptr) {
            dataPtr->connectorWireSwitchGraphics =
                    new ConnectorWireSwitch(getCanvasScene(), dataPtr->context->connected(), nullptr);
            dataPtr->connectorWireSwitchGraphics->attachConnectorWire(this);
            connect(dataPtr->connectorWireSwitchGraphics, SIGNAL(switchStatusChanged(bool)), this,
                    SLOT(onSwitchStatusChanged(bool)));
            // 此处设置一个较大的zvalue 是为了保证开关在连接线的线段上，
            // 因为连接线线段可以动态创建，避免后创建的线段在开关图层上，导致不能触发开关的点击事件
            canvasScene->addItem(dataPtr->connectorWireSwitchGraphics);
            dataPtr->connectorWireSwitchGraphics->setZValue(1000);
            dataPtr->connectorWireSwitchGraphics->userShow(true);
        }
        if (dataPtr->endSegment) {
            auto segmentDirection = dataPtr->endSegment->getSegmentDirection();
            int offset = 17.5;
            switch (segmentDirection) {
            case ConnectorWireSegment::kLeftToRight:
                dataPtr->connectorWireSwitchGraphics->setPos(
                        QPointF(dataPtr->lastEndPos.x() - offset, dataPtr->lastEndPos.y()));
                break;
            case ConnectorWireSegment::kBottomToTop:
                dataPtr->connectorWireSwitchGraphics->setPos(
                        QPointF(dataPtr->lastEndPos.x(), dataPtr->lastEndPos.y() + offset));
                break;
            case ConnectorWireSegment::kRightToLeft:
                dataPtr->connectorWireSwitchGraphics->setPos(
                        QPointF(dataPtr->lastEndPos.x() + offset, dataPtr->lastEndPos.y()));
                break;
            case ConnectorWireSegment::kTopToBottom:
                dataPtr->connectorWireSwitchGraphics->setPos(
                        QPointF(dataPtr->lastEndPos.x(), dataPtr->lastEndPos.y() - offset));
                break;
            default: {
                dataPtr->connectorWireSwitchGraphics->userShow(false);
            }
            }
        }
    } else {
        if (dataPtr->connectorWireSwitchGraphics) {
            dataPtr->connectorWireSwitchGraphics->userShow(false);
        }
    }
}

void ConnectorWireGraphicsObject::adjustWireSignalNamePos()
{
    QString algorithmName = getAlgorithmName();
    if (algorithmName != ManhattaAlgorithmName) {
        return;
    }
    if (dataPtr->signalGraphicsObject && dataPtr->signalGraphicsObject->isVisible()) {
        bool isVertical = false;
        QPointF signalPos = getSignalNamePos(isVertical);
        if (!isVertical) {
            QRectF rc = dataPtr->signalGraphicsObject->boundingRect();
            signalPos.setX(signalPos.x() - rc.width() / 2);
        }
        dataPtr->signalGraphicsObject->setPos(signalPos);
    }
}

Utility::WireSegmentDirection
ConnectorWireGraphicsObject::getRegulationControllerDirection(ConnectorWireRegulationController *controller)
{
    int pointSize = dataPtr->points.size();
    if (!controller || pointSize < 4) {
        return Utility::kInvalid;
    }
    if (controller == dataPtr->inputController) {
        return Utility::getSegmentDirection(dataPtr->points[pointSize - 1].toPoint(),
                                            dataPtr->points[pointSize - 2].toPoint());
    } else if (controller == dataPtr->outputController) {
        return Utility::getSegmentDirection(dataPtr->points[1].toPoint(), dataPtr->points[0].toPoint());
    } else {
        return Utility::kInvalid;
    }
}

ConnectorWireRegulationController *ConnectorWireGraphicsObject::getConnectorWireRegulationController(QString portType)
{
    if (portType == PortOutputType) {
        return dataPtr->outputController;
    }
    if (portType == PortInputType) {
        return dataPtr->inputController;
    }
    return nullptr;
}
