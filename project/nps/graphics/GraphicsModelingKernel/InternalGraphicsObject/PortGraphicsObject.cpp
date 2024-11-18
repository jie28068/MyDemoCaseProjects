#include "PortGraphicsObject.h"
#include "CanvasContext.h"
#include "ConnectorWireContext.h"
#include "ConnectorWireGraphicsObject.h"
#include "ConnectorWirePrompt.h"
#include "ConnectorWireRegulationController.h"
#include "ElectricalElementDirectConnectCommand.h"
#include "GraphicsKernelDefinition.h"
#include "ICanvasScene.h"
#include "ICanvasView.h"
#include "PortContext.h"
#include "PortPositionCommand.h"
#include "SourceGraphicsObject.h"
#include "SourceProxy.h"
#include "TransformCommand.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"
#include "graphicsmodelingkernel.h"

#include <QAbstractSlider>
#include <QCursor>
#include <QDateTime>
#include <QDebug>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QScrollBar>

PortGraphicsObject::PortGraphicsObject(ICanvasScene *canvasScene, SourceGraphicsObject *sourceGraphics,
                                       QSharedPointer<PortContext> context, QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent)
{
    dataPtr.reset(new PortGraphicsObjectPrivate());
    dataPtr->sourceGraphics = sourceGraphics;
    dataPtr->context = context;

    setAcceptHoverEvents(true);
    setFlags(ItemIsFocusable | ItemSendsGeometryChanges);
    setLayerFlag(GraphicsLayer::kPortGraphicsLayer);

    if (context) {
        connect(context.data(), &PortContext::portChange, this, &PortGraphicsObject::onPortChanged);
    }
}

PortGraphicsObject::~PortGraphicsObject() { }

int PortGraphicsObject::type() const
{
    return kPortGraphics;
}

bool PortGraphicsObject::canMove()
{
    return true;
}

bool PortGraphicsObject::isCanLinkWithClickedPort()
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return false;
    }

    PortGraphicsObject *mouseClickedPort = canvasScene->getMouseClickedPort();
    if (mouseClickedPort) {
        if (mouseClickedPort != this && (mouseClickedPort->isOutputPort() != isOutputPort() && canLink())) {
            return true;
        }
    }
    return false;
}

bool PortGraphicsObject::isMouseHover()
{
    return dataPtr->isHover;
}

void PortGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */) { }

QRectF PortGraphicsObject::boundingRect() const
{
    if (!dataPtr->context.isNull()) {
        if (dataPtr->context->size().width() > 0 && dataPtr->context->size().height() > 0) {
            return QRectF(-dataPtr->context->size().width() / 2.f, -dataPtr->context->size().height() / 2.f,
                          dataPtr->context->size().width(), dataPtr->context->size().height());
        } else {
            return QRectF(-5, -5, 10, 10);
        }

    } else {
        return QRectF(-5, -5, 10, 10);
    }
}

QString PortGraphicsObject::id()
{
    if (!dataPtr->context.isNull()) {
        return dataPtr->context->uuid();
    }
    return "";
}

void PortGraphicsObject::updatePosition()
{
    if (nullptr == dataPtr->sourceGraphics) {
        return;
    }
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }
    QPointF alignPos;
    QRectF sourceRect = dataPtr->sourceGraphics->boundingRect();
    int gridSpace = canvasContext->gridSpace();
    if (!dataPtr->context->isAbsolutePos()) {
        QPointF newPos = QPointF(sourceRect.left() + dataPtr->context->pos().x() * sourceRect.width(),
                                 sourceRect.top() + dataPtr->context->pos().y() * sourceRect.height());
        alignPos = dataPtr->sourceGraphics->transform().map(newPos);
        alignPos = Utility::pointAlignmentToGrid(alignPos, gridSpace);
        setPos(alignPos);
    } else {
        PSourceProxy sourceProxy = dataPtr->sourceGraphics->getSourceProxy();

        QPointF pos = dataPtr->context->pos();
        if (sourceProxy->prototypeName() == "DotBusbar") {
            // 点状母线的端口坐标永远是点状母线的原点
            //
            alignPos = QPointF(0, 0);
        } else {
            alignPos = dataPtr->sourceGraphics->transform().map(pos);
        }
        alignPos = Utility::pointAlignmentToGrid(alignPos, gridSpace);
        setPos(alignPos);
    }

    GraphicsLayer::updatePosition();
}

void PortGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    dataPtr->isHover = true;
    ICanvasScene *canvasScene = getCanvasScene();
    if (!canvasScene)
        return GraphicsLayer::hoverEnterEvent(event);

    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return GraphicsLayer::hoverEnterEvent(event);
    }

    if (!canvasContext->canModify()) {
        return GraphicsLayer::hoverEnterEvent(event);
    }

    if (canLink()) {
        setCursor(QCursor(QPixmap(":/GraphicsModelingKernel/cursorShape")));
    }

    if (isCanLinkWithClickedPort()) {
        dataPtr->canCreateConnectorWire = true;
        canvasScene->setPromptWirePorts(this, canvasScene->getMouseClickedPort());
    }

    GraphicsLayer::hoverEnterEvent(event);
}

void PortGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    dataPtr->isHover = false;
    ICanvasScene *canvasScene = getCanvasScene();
    if (!canvasScene)
        return GraphicsLayer::hoverLeaveEvent(event);

    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return GraphicsLayer::hoverLeaveEvent(event);
    }

    if (!canvasContext->canModify()) {
        return GraphicsLayer::hoverLeaveEvent(event);
    }

    for each (auto wire in getLinkedConnectorWireList()) {
        wire->setFlag(ItemIsSelectable, true);
    }

    if (dataPtr->canCreateConnectorWire) {
        dataPtr->canCreateConnectorWire = false;
        canvasScene->setPromptWirePorts(nullptr, nullptr);
    }

    setCursor(Qt::ArrowCursor);
    GraphicsLayer::hoverLeaveEvent(event);
}

void PortGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    ICanvasScene *canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }

    canvasScene->resetSubsystemGraphics();

    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }

    if (!canvasContext->canModify()) {
        return;
    }
    if (!canLink()) {
        GraphicsLayer::mousePressEvent(event);
        return;
    }

    // 在已经按住鼠标左键或者右键的情况下
    if (dataPtr->currentDrawingConnectorWire) {
        event->accept();
        return;
    }

    // 当前端口可以与场景中点击的端口相连接的时候 不创建临时连接线
    if (dataPtr->canCreateConnectorWire) {
        event->accept();
        return;
    }

    canvasScene->clearSelection();

    canvasScene->setMouseClickedPort(this);

    dataPtr->currentDrawingConnectorWire = new ConnectorWireGraphicsObject(getCanvasScene(), nullptr);

    // 新创建的连接线和当前端口连接起来
    dataPtr->currentDrawingConnectorWire->linkPortGraphicsObject(this);
    // 把当前新建的连接线添加到场景
    canvasScene->addConnectorWireGraphicsObject(dataPtr->currentDrawingConnectorWire);

    // 设置当前移动的ConnectorWireRegulationController为hover状态，以实现在移动中匹配
    if (canvasContext->isSupportSuspendedConnectorWire()) {
        ConnectorWireRegulationController *controller = nullptr;
        if (isOutputPort()) {
            controller = dataPtr->currentDrawingConnectorWire->getConnectorWireRegulationController(PortInputType);
        } else {
            controller = dataPtr->currentDrawingConnectorWire->getConnectorWireRegulationController(PortOutputType);
        }
        if (controller) {
            controller->setHoverFlag(true);
        }
    }

    event->accept();
}

void PortGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    ICanvasScene *canvasScene = getCanvasScene();
    if (!canvasScene)
        return;

    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }

    if (!canvasContext->canModify()) {
        return;
    }
    if (nullptr == dataPtr->currentDrawingConnectorWire) {
        QGraphicsObject::mouseMoveEvent(event);
        return;
    }

    // 实现连线的时候，视图的滚动条跟随鼠标滚动
    QPointF pos = event->scenePos();
    static QDateTime lastTime = QDateTime::currentDateTime();
    QList<QGraphicsView *> views = canvasScene->views();
    for each (QGraphicsView *view in views) {
        ICanvasView *canvasView = dynamic_cast<ICanvasView *>(view);
        if (canvasView) {
            QRectF viewPortRc = canvasView->viewport()->rect();
            QRectF viewSceneRc = canvasView->mapToScene(viewPortRc.toRect()).boundingRect();
            if (!viewSceneRc.contains(pos)) {
                QDateTime currentTime = QDateTime::currentDateTime();
                qint64 cost_time = lastTime.msecsTo(currentTime);
                if (cost_time >= 80) {
                    lastTime = currentTime;
                    if (pos.y() > viewSceneRc.bottom()) {
                        canvasView->verticalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepAdd);
                    }
                    if (pos.y() < viewSceneRc.top()) {
                        canvasView->verticalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepSub);
                    }
                    if (pos.x() < viewSceneRc.left()) {
                        canvasView->horizontalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepSub);
                    }
                    if (pos.x() > viewSceneRc.right()) {
                        canvasView->horizontalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepAdd);
                    }
                }
            }
        }
    }

    event->accept();

    // 高亮端口检测
    checkAvaliablePort(event->scenePos());

    QGraphicsObject::mouseMoveEvent(event);
}

void PortGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    ICanvasScene *canvasScene = getCanvasScene();
    if (!canvasScene)
        return;
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }

    // 2023.10.24 如果两个连接线的ConnectorWireRegulationController图元发生了碰撞连接，这个时候移动就不处理了
    if (canvasContext->isSupportSuspendedConnectorWire() && dataPtr->currentDrawingConnectorWire
        && dataPtr->currentDrawingConnectorWire->isConnectedWithPort()) {
        ConnectorWireRegulationController *controller = nullptr;
        if (isOutputPort()) {
            controller = dataPtr->currentDrawingConnectorWire->getConnectorWireRegulationController(PortInputType);
        } else {
            controller = dataPtr->currentDrawingConnectorWire->getConnectorWireRegulationController(PortOutputType);
        }
        if (controller) {
            auto matchedController = controller->getMatchedRegulationController();
            controller->matchAnotherRegulationController(nullptr);
            if (matchedController) {
                canvasScene->deleteConnectorWireGraphicsObject(matchedController->getConnectorWire());
                matchedController->matchAnotherRegulationController(nullptr);
                // 连接成功，把连接线上下文添加到画板
                canvasContext->addConnectWireContext(dataPtr->currentDrawingConnectorWire->getConnectorWireContext());

                // 添加到AddConnectorWireCommand
                canvasScene->addConnectorWireGraphicsObject(dataPtr->currentDrawingConnectorWire, true);

                dataPtr->currentDrawingConnectorWire = nullptr;
                canvasScene->setMouseClickedPort(nullptr);
                return;
            }
        }
    }

    // end

    if (!canvasContext->canModify()) {
        return;
    }

    if (dataPtr->canCreateConnectorWire) {
        CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(getCanvasScene());
        if (defaultScene) {
            defaultScene->createNewConnectWireByPrompt();
        }
        event->accept();
        return;
    }

    if (dataPtr->currentDrawingConnectorWire) {
        PortGraphicsObject *currentPort = dataPtr->currentDrawingConnectorWire->getStartPortGraphics() != nullptr
                ? dataPtr->currentDrawingConnectorWire->getStartPortGraphics()
                : dataPtr->currentDrawingConnectorWire->getEndPortGraphics();
        QPointF scenePos = event->scenePos();
        QRectF detectionArea = QRectF(scenePos, QSize(8, 8));
        detectionArea.moveCenter(scenePos);
        QList<QGraphicsItem *> items = canvasScene->items(detectionArea);
        foreach (QGraphicsItem *item, items) {
            if (item->type() == kPortGraphics) { // 命中图元为端口
                PortGraphicsObject *portGraphics = dynamic_cast<PortGraphicsObject *>(item);
                if (portGraphics && portGraphics != this) {
                    dataPtr->currentDrawingConnectorWire->linkPortGraphicsObject(portGraphics);
                    if (isBeDeletedModule(item)) {
                        dataPtr->currentDrawingConnectorWire->clearLinks();
                        canvasScene->deleteConnectorWireGraphicsObject(dataPtr->currentDrawingConnectorWire);
                        blog(QString(tr("The module prototype has been deleted or modified and cannot be "
                                        "connected"))); // 模块原型已被删除或修改，无法连接
                    }
                }
                if (!dataPtr->currentDrawingConnectorWire->isConnectedWithPort()) {
                    auto portContext = portGraphics->getPortContext();
                    auto currentPortContext = currentPort->getPortContext();
                    if (!portContext || !currentPortContext) {
                        continue;
                    }
                    if (portContext->type() == PortOutputType && currentPortContext->type() == PortOutputType
                        && portContext != currentPortContext) {
                        if (canvasContext->type() == CanvasContext::kElectricalType
                            || canvasContext->type() == CanvasContext::kElecUserDefinedType) {
                            // 2023.08.13 add by liwenyu
                            // 电气元件之间支持直接连接，连接时动态生成点状母线，元件连接到生成的点状母线上
                            if (portGraphics->getSourceProxy() != currentPort->getSourceProxy()) {
                                // 如果两个端口属于同一个电气元件，不允许连接
                                bool isVertical = false;
                                QPointF busbarPos = dataPtr->currentDrawingConnectorWire->getSignalNamePos(isVertical);
                                ElectricalElementDirectConnectCommand *command =
                                        new ElectricalElementDirectConnectCommand(canvasScene, nullptr);

                                ElectricalElementDirectConnectCommand::ElectricalElementInfo info1;
                                info1.sourceUUID = portGraphics->getSourceProxy()->uuid();
                                info1.portUUID = portContext->uuid();

                                ElectricalElementDirectConnectCommand::ElectricalElementInfo info2;
                                info2.sourceUUID = currentPort->getSourceProxy()->uuid();
                                info2.portUUID = currentPortContext->uuid();

                                dataPtr->currentDrawingConnectorWire->clearLinks();
                                canvasScene->deleteConnectorWireGraphicsObject(dataPtr->currentDrawingConnectorWire);
                                dataPtr->currentDrawingConnectorWire = nullptr;
                                canvasScene->setMouseClickedPort(nullptr);

                                command->setElementInfos(busbarPos, info1, info2);
                                canvasScene->getUndoStack()->push(command);

                                event->accept();
                                return;
                            }

                            // end
                        } else {
                            if (portGraphics != this) {
                                blog(QString(tr(
                                        "The output port cannot be connected to the output port"))); // 输出端口不能连接输出端口
                            }
                        }
                    } else if (portContext->type() == PortInputType && currentPortContext->type() == PortInputType) {
                        if (portGraphics != this) {

                            blog(QString(tr(
                                    "The input port cannot be connected to the input port"))); // 输入端口不能连接输入端口
                        }
                    } else if (portContext->type() == PortInputType
                               && portGraphics->getLinkedConnectorWireList().size() >= 1
                               && canvasContext->type() != CanvasContext::kElectricalType) {
                        blog(QString(tr("The input port cannot be connected more than once"))); // 输入端口不能多次连线
                    }
                };
            } else if (item->type()
                       == kConnectorWireGraphics) { // 命中单元为连接线 则新增一条连接线，连接线的起点为落点连接线的起点
                ConnectorWireGraphicsObject *connectorGraphics = dynamic_cast<ConnectorWireGraphicsObject *>(item);
                BusinessHooksServer *hooksserver = canvasContext->getBusinessHooksServer();
                bool sourcegraphics = hooksserver->isExistBlock(
                        dataPtr->sourceGraphics->getSourceProxy()); // 要连接的模块原型是否删除
                if (connectorGraphics && connectorGraphics->isConnectedWithPort()
                    && dataPtr->linkedConnectorWireGraphicsObject.indexOf(connectorGraphics) == -1) {
                    dataPtr->currentDrawingConnectorWire->linkPortGraphicsObject(
                            connectorGraphics->getStartPortGraphics());
                    bool isStartSourceProxyExist =
                            hooksserver->isExistBlock(connectorGraphics->getStartPortGraphics()
                                                              ->getSourceGraphics()
                                                              ->getSourceProxy()); // 连接的输出端模块原型是否删除
                    if (!isStartSourceProxyExist || !sourcegraphics) {
                        dataPtr->currentDrawingConnectorWire->clearLinks();
                        canvasScene->deleteConnectorWireGraphicsObject(dataPtr->currentDrawingConnectorWire);
                        blog(QString(tr("The module prototype has been deleted or modified and cannot be "
                                        "connected"))); // 模块原型已被删除或修改，无法连接
                    }
                }
            } else { // 模块是否可动态增加、删除端口

                GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
                if (layer) {
                    auto proxyGraphics = layer->getTransformProxyGraphicsObject();
                    if (proxyGraphics) {
                        SourceGraphicsObject *sourceGraphics = proxyGraphics->getSourceGraphicsObject();
                        auto sourceProxy = proxyGraphics->getSourceProxy();
                        if (sourceGraphics && sourceProxy && sourceProxy->isSupportAddOrDelPort()) {
                            PortGraphicsObject *portGraphics = sourceGraphics->addPortGraphicsObject(scenePos);
                            dataPtr->currentDrawingConnectorWire->linkPortGraphicsObject(portGraphics);
                        }
                    }
                }
            }
            if (dataPtr->currentDrawingConnectorWire->isConnectedWithPort()) {
                break;
            }
        }
        if (!dataPtr->currentDrawingConnectorWire->isConnectedWithPort()) {
            auto points = dataPtr->currentDrawingConnectorWire->getPoints();
            if (points.isEmpty() || !canvasContext->isSupportSuspendedConnectorWire()) {
                dataPtr->currentDrawingConnectorWire->clearLinks();
                // 鼠标松开时，连接线还没有分别连接2个端口，则把连接线从场景中移除
                canvasScene->deleteConnectorWireGraphicsObject(dataPtr->currentDrawingConnectorWire);
            } else {
                canvasContext->addConnectWireContext(dataPtr->currentDrawingConnectorWire->getConnectorWireContext());
                canvasScene->setMouseClickedPort(nullptr);
            }
            dataPtr->currentDrawingConnectorWire = nullptr;

        } else {
            // 连接成功，把连接线上下文添加到画板
            canvasContext->addConnectWireContext(dataPtr->currentDrawingConnectorWire->getConnectorWireContext());

            // 添加到AddConnectorWireCommand
            canvasScene->addConnectorWireGraphicsObject(dataPtr->currentDrawingConnectorWire, true);

            dataPtr->currentDrawingConnectorWire = nullptr;
            canvasScene->setMouseClickedPort(nullptr);
        }
        event->accept();
    }

    lightMovingHoverPortGraphics();
}

QVariant PortGraphicsObject::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case QGraphicsItem::ItemPositionHasChanged: {

    } break;
    default: {
    }
    }
    return GraphicsLayer::itemChange(change, value);
}

void PortGraphicsObject::setTransFormCommand(QList<QTransform> &oldTransFormList, QList<QTransform> &newTransFormList)
{
    if (oldTransFormList.size() != 5 || newTransFormList.size() != 5 || oldTransFormList == newTransFormList) {
        return;
    }

    auto transformProxyGraphics = getTransformProxyGraphicsObject();
    if (!transformProxyGraphics) {
        return;
    }
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }
    auto undoStack = canvasScene->getUndoStack();
    if (!undoStack) {
        return;
    }

    TransformCommand *command = new TransformCommand(getCanvasScene());
    command->setOldTransforms(transformProxyGraphics->id(), oldTransFormList[0], oldTransFormList[1],
                              oldTransFormList[2], oldTransFormList[3], oldTransFormList[4]);
    command->setNewTransforms(transformProxyGraphics->id(), newTransFormList[0], newTransFormList[1],
                              newTransFormList[2], newTransFormList[3], newTransFormList[4]);

    if (!command->isEmpty()) {
        undoStack->push(command);
    } else {
        delete command;
    }
}

void PortGraphicsObject::keyPressEvent(QKeyEvent *event)
{
    ICanvasScene *canvasScene = getCanvasScene();
    if (!canvasScene)
        return;

    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }

    if (!canvasContext->canModify()) {
        return;
    }

    GraphicsLayer::keyPressEvent(event);
}

void PortGraphicsObject::linkChanged() { }

void PortGraphicsObject::autoLayout() { }

QRectF PortGraphicsObject::portNameRect()
{
    return QRectF();
}

void PortGraphicsObject::resetPortAnchor() { }

QSharedPointer<PortContext> PortGraphicsObject::getPortContext() const
{
    return dataPtr->context;
}

SourceGraphicsObject *PortGraphicsObject::getSourceGraphics()
{
    return dataPtr->sourceGraphics;
}

void PortGraphicsObject::linkConnectorWire(ConnectorWireGraphicsObject *connectorWire)
{
    if (nullptr == connectorWire) {
        return;
    }
    int index = dataPtr->linkedConnectorWireGraphicsObject.indexOf(connectorWire);
    if (-1 == index) {
        dataPtr->linkedConnectorWireGraphicsObject.append(connectorWire);
    }

    linkChanged();
}

void PortGraphicsObject::unLinkConnectorWire(ConnectorWireGraphicsObject *connectorWire)
{
    if (nullptr == connectorWire) {
        return;
    }

    int index = dataPtr->linkedConnectorWireGraphicsObject.indexOf(connectorWire);
    if (index != -1) {
        dataPtr->linkedConnectorWireGraphicsObject.removeAt(index);
    }
    // 连接线断开时，当前端口时输出端口，则取消所连接的连接线的高亮显示状态
    if (dataPtr->context->type() == PortOutputType) {
        for each (auto wire in dataPtr->linkedConnectorWireGraphicsObject) {
            if (wire) {
                wire->enableHighLightHomologous(false);
            }
        }
    }
    deleteGraphicsObjectSelf();

    linkChanged();
}

QPointF PortGraphicsObject::getLinkPos()
{
    QRectF rect = boundingRect();
    QRectF portSceneRect = mapToScene(rect).boundingRect();
    QPointF centerPos = portSceneRect.center();
    return centerPos;
}

QPointF PortGraphicsObject::getCenterPos()
{
    QRectF rect = boundingRect();
    QRectF portSceneRect = mapToScene(rect).boundingRect();
    QPointF centerPos = portSceneRect.center();
    return centerPos;
}

QPointF PortGraphicsObject::getExternalPos()
{
    if (getPortContext().isNull()) {
        return getLinkPos();
    }
    QRectF portSceneRect = mapToScene(boundingRect()).boundingRect();
    QPointF centerPos = getLinkPos();
    int gridSpace = 10;
    ICanvasScene *canvasScene = dynamic_cast<ICanvasScene *>(scene());
    if (canvasScene) {
        gridSpace = canvasScene->getGridSpace();
    }
    Qt::AnchorPoint anchor = getAnchorPoint();
    switch (anchor) {
    case Qt::AnchorTop:
        return QPointF(centerPos.rx(), centerPos.ry() - 2 * gridSpace - portSceneRect.height() / 2);
    case Qt::AnchorRight:
        return QPointF(centerPos.rx() + 2 * gridSpace + portSceneRect.width() / 2, centerPos.ry());
    case Qt::AnchorBottom:
        return QPointF(centerPos.rx(), centerPos.ry() + 2 * gridSpace + portSceneRect.height() / 2);
    case Qt::AnchorLeft:
        return QPointF(centerPos.rx() - 2 * gridSpace - portSceneRect.width() / 2, centerPos.ry());
    }
    return centerPos;
}

Qt::AnchorPoint PortGraphicsObject::getAnchorPoint()
{
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    if (proxyGraphics) {
        auto sourceProxy = proxyGraphics->getSourceProxy();
        if (!sourceProxy) {
            Utility::getAnchorPoint(dataPtr->context->anchorPoint(), 0);
        }

        auto anchor = Utility::getAnchorPoint(dataPtr->context->anchorPoint(), proxyGraphics->getAngle());
        // 考虑翻转的情况,翻转的角度只会是180度翻转或者未翻转
        if (anchor == Qt::AnchorLeft || anchor == Qt::AnchorRight) {
            // 如果端口锚点方向是水平的，此时就需要考虑水平的翻转情况对应的就是按照Y轴的翻转
            bool isIdentity = sourceProxy->yAxisFlipTransform().isIdentity();
            if (!isIdentity) {
                if (anchor == Qt::AnchorLeft) {
                    anchor = Qt::AnchorRight;
                } else {
                    anchor = Qt::AnchorLeft;
                }
            }
        } else if (anchor == Qt::AnchorTop || anchor == Qt::AnchorBottom) {
            // 如果端口的锚点方向是垂直的，此时就需要考虑垂直方向的翻转情况，对应的及时按照X轴的翻转
            bool isIdentity = sourceProxy->xAxisFlipTransform().isIdentity();
            if (!isIdentity) {
                if (anchor == Qt::AnchorTop) {
                    anchor = Qt::AnchorBottom;
                } else {
                    anchor = Qt::AnchorTop;
                }
            }
        }
        return anchor;
    }
    return Utility::getAnchorPoint(dataPtr->context->anchorPoint(), 0);
}

QList<ConnectorWireGraphicsObject *> PortGraphicsObject::getLinkedConnectorWireList()
{
    return dataPtr->linkedConnectorWireGraphicsObject;
}

bool PortGraphicsObject::canLink()
{
    // 如果端口已经连接，并且不支持多连接线则直接返回不处理
    if (!dataPtr->context->supportMultipConnectorWire() && dataPtr->linkedConnectorWireGraphicsObject.size() > 0) {
        return false;
    }
    auto sourceProxy = getSourceProxy();
    // 如果端口已经链接，并且为动态可删除增加，则直接返回不处理
    if (sourceProxy && sourceProxy->isSupportAddOrDelPort() && dataPtr->linkedConnectorWireGraphicsObject.size() > 0) {
        return false;
    }
    return true;
}

void PortGraphicsObject::calcuteConnectorWireBranchPoints()
{
    int wireSize = dataPtr->linkedConnectorWireGraphicsObject.size();
    if (wireSize <= 0) {
        return;
    }
    cleanConnectorWireBranchPoints();
    // 当前端口时输出端口
    if (isOutputPort()) {

        auto wires = dataPtr->linkedConnectorWireGraphicsObject;
        if (wireSize <= 1) {
            return;
        }
        for each (auto wire in wires) {
            if (wire) {
                wire->cleanBranchPoints();
            }
        }
        for (int i = 0; i < wireSize; i++) {
            auto flagWire = wires[i];
            if (!flagWire) {
                continue;
            }
            for (int j = 0; j < wireSize; j++) {
                auto tempWire = wires[j];
                if (tempWire == flagWire) {
                    continue;
                }
                QPoint branchPt = Utility::getBranchPoint(flagWire->getPoints(), tempWire->getPoints());
                if (!branchPt.isNull()) {
                    flagWire->addBranchPoint(branchPt);
                }
            }
        }

    } else {
        auto wire = dataPtr->linkedConnectorWireGraphicsObject[0];
        auto outputPort = wire->getAnotherPort(this);
        if (outputPort && outputPort->isOutputPort()) {
            outputPort->calcuteConnectorWireBranchPoints();
        }
    }
}

void PortGraphicsObject::cleanConnectorWireBranchPoints()
{
    for each (ConnectorWireGraphicsObject *wire in dataPtr->linkedConnectorWireGraphicsObject) {
        if (wire) {
            // 查找该连接线的输出端口
            PortGraphicsObject *outputPortGraphics = wire->getOutputTypePortGraphics();
            if (!outputPortGraphics) {
                continue;
            }
            // 如果输出端口不是本端口，那么处理输出端口的连接线
            if (outputPortGraphics != this) {
                outputPortGraphics->cleanConnectorWireBranchPoints();
            } else {
                wire->cleanBranchPoints();
            }
        }
    }
}

void PortGraphicsObject::lightMovingHoverPortGraphics(QList<PortGraphicsObject *> portGraphics)
{
    for each (auto graphics in dataPtr->movingHoverPortGraphics) {
        graphics->update();
    }
    for each (auto graphics in portGraphics) {
        graphics->update();
    }
    dataPtr->movingHoverPortGraphics = portGraphics;
}

bool PortGraphicsObject::isOutputPort()
{
    return dataPtr->context->type() == PortOutputType;
}

void PortGraphicsObject::checkAvaliablePort(QPointF scenePos)
{
    if (!dataPtr->context) {
        return;
    }
    auto canvasContext = getCanvasContext();
    QList<PortGraphicsObject *> hoverPorts;

    QRectF detectionArea = QRectF(scenePos, QSize(8, 8));
    detectionArea.moveCenter(scenePos);
    QList<QGraphicsItem *> items = scene()->items(detectionArea);

    if (!dataPtr->currentDrawingConnectorWire) {
        foreach (QGraphicsItem *item, items) {
            if (item->type() == kPortGraphics) { // 命中图元为端口
                PortGraphicsObject *portGraphics = dynamic_cast<PortGraphicsObject *>(item);
                if (portGraphics && portGraphics != this) {
                    auto checkPortContext = portGraphics->getPortContext();
                    if (checkPortContext && portGraphics->canLink()
                        && dataPtr->context->type() != checkPortContext->type()) {
                        hoverPorts.append(portGraphics);
                        break;
                    }
                }
            }
        }
    } else {
        // 2023.10.24 如果两个连接线的ConnectorWireRegulationController图元发生了碰撞连接，这个时候移动就不处理了
        // 如果鼠标移动点离开了匹配的controller，那么继续处理鼠标移动逻辑
        if (canvasContext->isSupportSuspendedConnectorWire()
            && dataPtr->currentDrawingConnectorWire->isConnectedWithPort()) {
            ConnectorWireRegulationController *controller = nullptr;
            if (isOutputPort()) {
                controller = dataPtr->currentDrawingConnectorWire->getConnectorWireRegulationController(PortInputType);
            } else {
                controller = dataPtr->currentDrawingConnectorWire->getConnectorWireRegulationController(PortOutputType);
            }
            if (controller) {
                auto matchedController = controller->getMatchedRegulationController();
                if (matchedController) {
                    foreach (QGraphicsItem *item, items) {
                        if (item->type() == kConnectorRetulationControllerGraphics) { // 命中图元为端口
                            ConnectorWireRegulationController *hitController =
                                    dynamic_cast<ConnectorWireRegulationController *>(item);
                            if (hitController == controller) {
                                continue;
                            }
                            if (hitController == matchedController) {
                                return;
                            }
                        }
                    }
                }
            }
        }

        // end

        PortGraphicsObject *linkPort = nullptr;
        PortGraphicsObject *otherPort = dataPtr->currentDrawingConnectorWire->getAnotherPort(this);
        QString currentPortType = dataPtr->context->type();
        foreach (QGraphicsItem *item, items) {
            if (item->type() == kPortGraphics) { // 命中图元为端口
                PortGraphicsObject *portGraphics = dynamic_cast<PortGraphicsObject *>(item);
                if (portGraphics && portGraphics != this) {
                    auto checkPortContext = portGraphics->getPortContext();
                    if ((portGraphics->canLink() || portGraphics == otherPort)
                        && dataPtr->context->type() != checkPortContext->type()) {
                        hoverPorts.append(portGraphics);
                        linkPort = portGraphics;
                        if (dataPtr->currentDrawingConnectorWire && portGraphics != otherPort) {
                            dataPtr->currentDrawingConnectorWire->linkPortGraphicsObject(portGraphics);
                            break;
                        }
                    }
                }

            } else if (item->type() == kTransformProxyGraphics) {
                // 如果是代理图元
                TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
                if (proxyGraphics && proxyGraphics != getTransformProxyGraphicsObject()) {
                    // 获取代理图层的端口列表
                    QList<PortGraphicsObject *> ports = proxyGraphics->getPortGraphicsObjectList();

                    int maxDistance = 0xffffff;
                    if (otherPort) {
                        maxDistance = Utility::lineDistance(scenePos.toPoint(),
                                                            otherPort->mapToScene(otherPort->pos()).toPoint());
                        linkPort = otherPort;
                    }

                    for each (auto port in ports) {
                        if (port) {
                            auto tempPortContext = port->getPortContext();
                            if (!tempPortContext) {
                                continue;
                            }
                            QString portType = tempPortContext->type();
                            if (currentPortType != portType && (port->canLink() || port == otherPort)) {
                                qreal distance =
                                        Utility::lineDistance(scenePos.toPoint(), port->getLinkPos().toPoint());
                                if (distance < maxDistance) {
                                    linkPort = port;
                                    maxDistance = distance;
                                }
                            }
                        }
                    }

                    if (dataPtr->currentDrawingConnectorWire && linkPort && otherPort != linkPort) {
                        dataPtr->currentDrawingConnectorWire->linkPortGraphicsObject(linkPort);
                    }
                }
            } else if (item->type()
                       == kConnectorWireGraphics) { // 命中单元为连接线 则新增一条连接线，连接线的起点为落点连接线的起点
                if (currentPortType == PortOutputType) {
                    continue;
                }
                ConnectorWireGraphicsObject *connectorGraphics = dynamic_cast<ConnectorWireGraphicsObject *>(item);
                if (connectorGraphics && connectorGraphics->isConnectedWithPort()
                    && dataPtr->linkedConnectorWireGraphicsObject.indexOf(connectorGraphics) == -1) {
                    linkPort = connectorGraphics->getOutputTypePortGraphics();
                    if (dataPtr->currentDrawingConnectorWire && linkPort && otherPort != linkPort) {
                        dataPtr->currentDrawingConnectorWire->linkPortGraphicsObject(linkPort);
                    }
                    break;
                }
            }
        }

        if (!linkPort) {
            if (dataPtr->currentDrawingConnectorWire && otherPort) {
                qDebug() << "unlink";
                dataPtr->currentDrawingConnectorWire->unlinkPortGraphicsObject(otherPort);
            }
            // 设置当前连接中的连接线的移动坐标
            dataPtr->currentDrawingConnectorWire->setMovingPos(scenePos);
            dataPtr->currentDrawingConnectorWire->prepareGeometryChange();
        }
    }

    lightMovingHoverPortGraphics(hoverPorts);
}

void PortGraphicsObject::deleteGraphicsObjectSelf()
{
    QSharedPointer<SourceProxy> pSourceProxy = getSourceProxy();
    SourceGraphicsObject *pSourceGpraphics = getSourceGraphics();
    if (!pSourceProxy)
        return;

    if (!pSourceProxy->isSupportAddOrDelPort())
        return;

    // 端口图元删除
    pSourceGpraphics->deletePortGraphicsObject(this);
    // 数据删除
    pSourceProxy->delPortContext(dataPtr->context);
    dataPtr->linkedConnectorWireGraphicsObject.clear();
}

void PortGraphicsObject::deletePortGraphicsObjectSelf()
{
    QSharedPointer<SourceProxy> pSourceProxy = getSourceProxy();
    SourceGraphicsObject *pSourceGpraphics = getSourceGraphics();
    if (!pSourceProxy)
        return;

    // 数据删除
    pSourceProxy->delPortContext(dataPtr->context);
    // 端口图元删除
    pSourceGpraphics->deletePortGraphics(this);
}

bool PortGraphicsObject::isBeDeletedModule(QGraphicsItem *item)
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return false;
    }
    BusinessHooksServer *hooksserver = canvasContext->getBusinessHooksServer();
    if (!hooksserver) {
        return false;
    }
    bool isexist = hooksserver->isExistBlock(dataPtr->sourceGraphics->getSourceProxy());
    // 初始端sourceGraphics
    if (!isexist) {
        return true;
    }
    // 连接端sourceGraphics
    GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
    if (layer) {
        auto proxyGraphics = layer->getTransformProxyGraphicsObject();
        if (proxyGraphics == nullptr) {
            return true;
        }
        isexist = hooksserver->isExistBlock(proxyGraphics->getSourceProxy());
        if (!isexist) {
            return true;
        }
    }

    return false;
}

bool PortGraphicsObject::portIsMatchAutoConnect(PortGraphicsObject *port)
{
    if (!port) {
        return false;
    }
    // 端口类型是否配
    if (!portTypeIsMatchAutoConnect(port)) {
        return false;
    }
    // 端口锚点方向是否匹配
    if (!portAnchorIsMatchAutoConnect(port)) {
        return false;
    }
    bool ret = false;
    auto anchor = getAnchorPoint();
    QPoint ownerLinkPos = getLinkPos().toPoint();
    QPoint otherLinkPos = port->getLinkPos().toPoint();
    switch (anchor) {
    case Qt::AnchorLeft:
        if (ownerLinkPos.y() == otherLinkPos.y() && otherLinkPos.x() < ownerLinkPos.x()) {
            ret = true;
        }
        break;
    case Qt::AnchorTop:
        if (ownerLinkPos.x() == otherLinkPos.x() && otherLinkPos.y() < ownerLinkPos.y()) {
            ret = true;
        }
        break;
    case Qt::AnchorRight:
        if (ownerLinkPos.y() == otherLinkPos.y() && otherLinkPos.x() > ownerLinkPos.x()) {
            ret = true;
        }
        break;
    case Qt::AnchorBottom:
        if (ownerLinkPos.x() == otherLinkPos.x() && otherLinkPos.y() > ownerLinkPos.y()) {
            ret = true;
        }

    default:
        break;
    }

    return ret;
}

bool PortGraphicsObject::portAnchorIsMatchAutoConnect(PortGraphicsObject *port)
{
    if (!port) {
        return false;
    }
    auto ownerAnchor = getAnchorPoint();
    auto otherAnchor = port->getAnchorPoint();
    if (ownerAnchor == Qt::AnchorLeft && otherAnchor == Qt::AnchorRight) {
        return true;
    }
    if (ownerAnchor == Qt::AnchorTop && otherAnchor == Qt::AnchorBottom) {
        return true;
    }
    if (ownerAnchor == Qt::AnchorRight && otherAnchor == Qt::AnchorLeft) {
        return true;
    }
    if (ownerAnchor == Qt::AnchorBottom && otherAnchor == Qt::AnchorTop) {
        return true;
    }
    return false;
}

bool PortGraphicsObject::portTypeIsMatchAutoConnect(PortGraphicsObject *port)
{
    if (!port) {
        return false;
    }
    int flag = port->isOutputPort() + isOutputPort();
    return flag == 1;
}

QPointF PortGraphicsObject::getPropmtPos()
{
    QPointF pt = getCenterPos();
    int offset = 8;
    switch (getAnchorPoint()) {
    case Qt::AnchorLeft:
        pt.setX(pt.x() - offset);
        break;
    case Qt::AnchorTop:
        pt.setY(pt.y() - offset);
        break;
    case Qt::AnchorRight:
        pt.setX(pt.x() + offset);
        break;
    case Qt::AnchorBottom:
        pt.setY(pt.y() + offset);
        break;
    default: {
    }
    }
    return pt;
}

void PortGraphicsObject::onPortChanged()
{
    updatePosition();
}
