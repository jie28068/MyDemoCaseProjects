#include "SourceGraphicsObject.h"
#include "ActionManager.h"
#include "CanvasContext.h"
#include "GraphicsFactoryManager.h"
#include "GraphicsKernelDefinition.h"
#include "ICanvasGraphicsObjectFactory.h"
#include "ICanvasScene.h"
#include "PortContext.h"
#include "SourceGraphicsObjectPrivate.h"
#include "SourceProxy.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"

#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneContextMenuEvent>
#include <QPainter>

SourceGraphicsObject::SourceGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                           QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent)
{
    dataPtr.reset(new SourceGraphicsObjectPrivate());
    dataPtr->sourceProxy = source;
    dataPtr->scaleDirection = kScaleAll;
    dataPtr->xAlignmentType = kAlignmentGrid;
    dataPtr->yAlignmentType = kAlignmentGrid;
    dataPtr->isHover = false;

    setSourceBoundingRect();
    dataPtr->minimumSize = source->size(); // dataPtr->sourceBoundingRect.size();
    setLayerFlag(GraphicsLayer::kSourceGraphicsLayer);
    setFlag(ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);

    connect(source.data(), &SourceProxy::sigAddPortContext, this, &SourceGraphicsObject::onAddPort);
    connect(source.data(), &SourceProxy::sigDelPortContext, this, &SourceGraphicsObject::onDeletePort);

    SourceProperty &sourceProperty = source->getSourceProperty();
    connect(&sourceProperty, &NPSPropertyManager::propertyChanged, this,
            &SourceGraphicsObject::onSourcePropertyChanged);
}

SourceGraphicsObject::~SourceGraphicsObject()
{
    disconnect();
}

int SourceGraphicsObject::type() const
{
    return kSourceGraphics;
}

QString SourceGraphicsObject::id()
{
    if (!dataPtr->sourceProxy.isNull()) {
        return dataPtr->sourceProxy->uuid();
    }
    return "";
}

QSharedPointer<SourceProxy> SourceGraphicsObject::getSourceProxy() const
{
    return dataPtr->sourceProxy;
}

QRectF SourceGraphicsObject::getTransformBoundingRect() const
{
    if (dataPtr->sourceProxy.isNull()) {
        return QRectF(0, 0, 0, 0);
    }
    QTransform trans = dataPtr->sourceProxy->scaleTransform() * dataPtr->sourceProxy->rotateTransform()
            * dataPtr->sourceProxy->xAxisFlipTransform() * dataPtr->sourceProxy->yAxisFlipTransform()
            * dataPtr->sourceProxy->translateTransform();

    return trans.mapRect(boundingRect());
}

void SourceGraphicsObject::applyTransform()
{

    if (dataPtr->sourceProxy.isNull()) {
        return;
    }

    QTransform trans = dataPtr->sourceProxy->scaleTransform() * dataPtr->sourceProxy->rotateTransform()
            * dataPtr->sourceProxy->xAxisFlipTransform() * dataPtr->sourceProxy->yAxisFlipTransform()
            * dataPtr->sourceProxy->translateTransform();

    setTransform(trans);
}

void SourceGraphicsObject::setScaleDirection(SourceGraphicsObject::ScaleDirection direction)
{
    dataPtr->scaleDirection = direction;
}

SourceGraphicsObject::ScaleDirection SourceGraphicsObject::getScaleDirection()
{
    return dataPtr->scaleDirection;
}

SourceGraphicsObject::AlignmentType SourceGraphicsObject::getXAxisAlignment()
{
    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (!proxyGraphics) {
        return dataPtr->xAlignmentType;
    }
    int angle = proxyGraphics->getAngle();
    if (angle % 180 != 0) {
        return dataPtr->yAlignmentType;
    }
    return dataPtr->xAlignmentType;
}

void SourceGraphicsObject::setXAxisAlignment(SourceGraphicsObject::AlignmentType type)
{
    dataPtr->xAlignmentType = type;
}

SourceGraphicsObject::AlignmentType SourceGraphicsObject::getYAxisAlignment()
{
    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (!proxyGraphics) {
        return dataPtr->yAlignmentType;
    }
    int angle = proxyGraphics->getAngle();
    if (angle % 180 != 0) {
        return dataPtr->xAlignmentType;
    }
    return dataPtr->yAlignmentType;
}

void SourceGraphicsObject::setYAxisAlignment(SourceGraphicsObject::AlignmentType type)
{
    dataPtr->yAlignmentType = type;
}

void SourceGraphicsObject::setSourceBoundingRect(QRectF rect)
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext || !dataPtr->sourceProxy) {
        return;
    }
    QSizeF size = dataPtr->sourceProxy->size();
    if (rect.isNull()) {
        // 默认为资源的size大小
        size = Utility::sizeAlignmentToGrid(size, canvasContext->gridSpace());
        rect = QRectF(0, 0, size.width(), size.height());
    } else {
        size = rect.size();
    }

    QSizeF boudingRectSize = dataPtr->sourceBoundingRect.size();
    if (boudingRectSize == size) {
        return;
    }
    dataPtr->sourceBoundingRect = QRectF(0, 0, size.width(), size.height());
    dataPtr->sourceProxy->setSize(size);
}

void SourceGraphicsObject::adjustBoundingRect(QRectF rc)
{
    dataPtr->sourceBoundingRect = rc;
    // dataPtr->minimumSize = dataPtr->sourceBoundingRect.size();
}

bool SourceGraphicsObject::isHoverStatus()
{
    return dataPtr->isHover;
}

void SourceGraphicsObject::setHoverStatus(bool status)
{
    dataPtr->isHover = status;
}

void SourceGraphicsObject::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QSharedPointer<QMenu> pMenu(new QMenu);
    userMenu(pMenu);

    if (pMenu->isEmpty()) {
        return GraphicsLayer::contextMenuEvent(event);
    }
    auto canvasContext = getCanvasContext();
    if (!canvasContext || !dataPtr->sourceProxy) {
        return;
    }
    auto businessHook = canvasContext->getBusinessHooksServer();

    if (dataPtr->sourceProxy->moduleType() != GKD::SOURCE_MODULETYPE_ELECTRICAL) {
        if (businessHook) {
            if (!businessHook->checkBlockState(dataPtr->sourceProxy)) {
                // 模块状态为不可用
                pMenu->clear();
                // pMenu->addAction(ActionManager::getInstance().getAction(ActionManager::updateModule));
                pMenu->addAction(ActionManager::getInstance().getAction(ActionManager::Delete));
            }
        }
    }

    // action信号处理
    connect(ActionManager::getInstance().getAction(ActionManager::ElementProperty), SIGNAL(triggered()), this,
            SLOT(onOpenSourceProperty()), Qt::UniqueConnection);
    connect(ActionManager::getInstance().getAction(ActionManager::OpenModel), SIGNAL(triggered()), this,
            SLOT(viewDrawingBoard()));

    auto topItem = topLevelItem();
    if (topItem && !topItem->isSelected()) {
        auto canvasScene = getCanvasScene();
        if (canvasScene) {
            canvasScene->clearSelection();
        }
        topItem->setSelected(true);
    }
    checkIsHaveConnectorWire();
    pMenu->setObjectName("uniformStyleMenu");
    pMenu->exec(event->screenPos());
    event->accept();
    // 移除信号处理，因为如果此处不断开信号处理，在右键其他资源的时候，当前资源也会响应该action事件
    disconnect(ActionManager::getInstance().getAction(ActionManager::ElementProperty), 0, this, 0);
    disconnect(ActionManager::getInstance().getAction(ActionManager::OpenModel), 0, this, 0);
}

void SourceGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    dataPtr->isHover = true;
    GraphicsLayer::hoverEnterEvent(event);
}

void SourceGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    dataPtr->isHover = false;
    GraphicsLayer::hoverLeaveEvent(event);
}

void SourceGraphicsObject::onSourcePropertyChanged(QString key, QVariant oldValue, QVariant newValue) { }

void SourceGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    GraphicsLayer::paint(painter, option, widget);
}

bool SourceGraphicsObject::checkIsHaveConnectorWire()
{
    bool check = false;
    TransformProxyGraphicsObject *proxyLayer = getTransformProxyGraphicsObject();
    QList<PortGraphicsObject *> portList = proxyLayer->getPortGraphicsObjectList();
    foreach (PortGraphicsObject *port, portList) {
        if (port->getLinkedConnectorWireList().isEmpty()) {
            continue;
        } else {
            check = true;
            break;
        }
    }

    return check;
}

void SourceGraphicsObject::viewDrawingBoard()
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return;
    }
    auto businessHook = canvasContext->getBusinessHooksServer();
    if (!businessHook) {
        return;
    }
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    if (proxyGraphics && proxyGraphics->isSelected()) {
        businessHook->openModelBoard(dataPtr->sourceProxy.data());
    }
}

void SourceGraphicsObject::onOpenSourceProperty()
{
    if (!dataPtr->sourceProxy) {
        return;
    }
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    if (proxyGraphics && proxyGraphics->isSelected()) {
        dataPtr->sourceProxy->emitEditProperty();
    }
}

void SourceGraphicsObject::onAddPort(PPortContext portContext)
{
    if (!portContext || !dataPtr->sourceProxy) {
        return;
    }
    auto transformProxyGraphicsObject = getTransformProxyGraphicsObject();
    if (!transformProxyGraphicsObject) {
        return;
    }
    PortGraphicsObject *portGraphics = transformProxyGraphicsObject->getPortGraphicsObject(portContext->uuid());
    if (portGraphics) {
        // 已经存在该端口图元，不再添加
        return;
    }
    QString moduleType = dataPtr->sourceProxy->moduleType();
    QSharedPointer<ICanvasGraphicsObjectFactory> factory = GraphicsFactoryManager::getInstance().getFactory(moduleType);
    auto canvasScene = getCanvasScene();
    if (factory && canvasScene) {
        portGraphics = factory->createPortGraphicsObject(canvasScene, this, portContext);
        portGraphics->setParentItem(transformProxyGraphicsObject);
        portGraphics->updatePosition();
        portGraphics->createInternalGraphics();
    }
}

void SourceGraphicsObject::onDeletePort(PPortContext portContext)
{
    auto transformProxyGraphicsObject = getTransformProxyGraphicsObject();
    auto portGraphics = transformProxyGraphicsObject->getPortGraphicsObject(portContext->uuid());
    if (portGraphics) {
        deletePortGraphicsObject(portGraphics);
    }
}

QRectF SourceGraphicsObject::boundingRect() const
{
    return dataPtr->sourceBoundingRect;
}

void SourceGraphicsObject::userMenu(QSharedPointer<QMenu> menu)
{
    auto canvasContext = getCanvasContext();
    if (!dataPtr->sourceProxy || !canvasContext) {
        return;
    }
    BusinessHooksServer *businessHook = canvasContext->getBusinessHooksServer();

    menu->setMinimumWidth(150);
    menu->addAction(ActionManager::getInstance().getAction(ActionManager::ElementProperty));

    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (proxyGraphics) {
        auto ports = proxyGraphics->getPortGraphicsObjectList();
        bool hasConnected = false;
        for (auto port : ports) {
            if (port->getLinkedConnectorWireList().size() > 0) {
                hasConnected = true;
                break;
            }
        }
        if (hasConnected) {
            menu->addSeparator();
            menu->addAction(ActionManager::getInstance().getAction(ActionManager::BreakLinkLine));
        }
    }

    bool canModify = canvasContext->canModify();
    if (!canModify) {
        // 冻结模式下注释模块不可编辑
        if (dataPtr->sourceProxy->moduleType() == GKD::SOURCE_MODULETYPE_ANNOTATION) {
            ActionManager::getInstance().getAction(ActionManager::ElementProperty)->setEnabled(false);
        } else {
            ActionManager::getInstance().getAction(ActionManager::ElementProperty)->setEnabled(true);
        }
        ActionManager::getInstance().getAction(ActionManager::BreakLinkLine)->setEnabled(false);
    } else {
        ActionManager::getInstance().getAction(ActionManager::ElementProperty)->setEnabled(true);
        ActionManager::getInstance().getAction(ActionManager::BreakLinkLine)->setEnabled(true);
    }

    menu->addSeparator();
    menu->addAction(ActionManager::getInstance().getAction(ActionManager::Copy));
    menu->addAction(ActionManager::getInstance().getAction(ActionManager::Cut));
    menu->addAction(ActionManager::getInstance().getAction(ActionManager::Delete));
    // 电气模块增加 标记为直通 标记为禁用 取消直通/禁用标记三个action， in out不能设置直通禁用
    QString protypeName = dataPtr->sourceProxy->prototypeName();
    if (dataPtr->sourceProxy->moduleType() == GKD::SOURCE_MODULETYPE_CONTROL && canModify && protypeName != "In"
        && protypeName != "Out") {
        menu->addAction(ActionManager::getInstance().getAction(ActionManager::MarkThrough));
        menu->addAction(ActionManager::getInstance().getAction(ActionManager::MarkDisable));
        menu->addAction(ActionManager::getInstance().getAction(ActionManager::Unmark));
        if (dataPtr->sourceProxy->getCommentState() == "through") {
            ActionManager::getInstance().getAction(ActionManager::MarkThrough)->setEnabled(false);
            ActionManager::getInstance().getAction(ActionManager::MarkDisable)->setEnabled(true);
            ActionManager::getInstance().getAction(ActionManager::Unmark)->setEnabled(true);
        } else if (dataPtr->sourceProxy->getCommentState() == "disable") {
            ActionManager::getInstance().getAction(ActionManager::MarkThrough)->setEnabled(true);
            ActionManager::getInstance().getAction(ActionManager::MarkDisable)->setEnabled(false);
            ActionManager::getInstance().getAction(ActionManager::Unmark)->setEnabled(true);
        } else {
            ActionManager::getInstance().getAction(ActionManager::MarkThrough)->setEnabled(true);
            ActionManager::getInstance().getAction(ActionManager::MarkDisable)->setEnabled(true);
            ActionManager::getInstance().getAction(ActionManager::Unmark)->setEnabled(false);
        }
    }
    menu->addSeparator();
    menu->addAction(ActionManager::getInstance().getAction(ActionManager::RotateAll));

    QMenu *secMenu = new QMenu(menu.data());
    secMenu->setMinimumWidth(220);
    secMenu->addAction(ActionManager::getInstance().getAction(ActionManager::RotateClockwise));
    secMenu->addAction(ActionManager::getInstance().getAction(ActionManager::RotateAntiClockwise));
    secMenu->addAction(ActionManager::getInstance().getAction(ActionManager::Rotate180));
    ActionManager::getInstance().getAction(ActionManager::RotateAll)->setMenu(secMenu);
    // 如果是组合控制模块，说明有下一层画板
    bool isCtrlCombined = false;
    if (businessHook) {
        isCtrlCombined = businessHook->isBlockCtrlCombined(dataPtr->sourceProxy);
    }
    /* if (isCtrlCombined) {
        QAction *action = ActionManager::getInstance().getAction(ActionManager::OpenModel);
        if (action) {
            menu->addAction(action);
        }
    } */
    menu->setObjectName("uniformStyleMenu");
    secMenu->setObjectName("uniformStyleMenu");
}

QSizeF SourceGraphicsObject::getMinimumSize() const
{
    return dataPtr->minimumSize;
}

PortGraphicsObject *SourceGraphicsObject::addPortGraphicsObject(QPointF scenePos)
{
    if (!dataPtr->sourceProxy) {
        return nullptr;
    }
    if (!dataPtr->sourceProxy->isSupportAddOrDelPort()) {
        return nullptr;
    }
    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return nullptr;
    }

    // 端口在画板上的位置
    QPointF gridPos = QPointF(qRound(scenePos.rx() / 10.0) * 10.0, qRound(scenePos.ry() / 10.0) * 10.0);
    // 端口实际在母线上的位置
    auto parentItemObject = parentItem();
    if (!parentItemObject) {
        return nullptr;
    }
    QRectF parentRec = parentItemObject->boundingRect();
    QPointF busPortAbsolutePos = parentItemObject->mapFromScene(gridPos);
    if (!parentRec.contains(busPortAbsolutePos)) {
        return nullptr;
    }
    // 端口相对与母线框的位置
    QPointF busPortRelativePos = this->mapFromScene(gridPos);

    QSharedPointer<PortContext> portContext =
            PPortContext(new PortContext(dataPtr->sourceProxy->uuid(), Utility::createUUID()));

    BusinessHooksServer *businessHooks = canvasContext->getBusinessHooksServer();
    if (businessHooks) {
        businessHooks->setCreatePortContext(portContext);
    }
    portContext->setSize(QSize(GKD::PORT_SIZE, GKD::PORT_SIZE));
    // 如果是点状母线，位置设置0,0
    if (dataPtr->sourceProxy->prototypeName() == "DotBusbar") {
        busPortRelativePos = QPointF(0, 0);
    }
    portContext->setPos(busPortRelativePos);
    portContext->setIsAbsolutePos(true);

    PortGraphicsObject *portGraphics = addPortGraphicsObject(portContext);

    return portGraphics;
}

PortGraphicsObject *SourceGraphicsObject::addPortGraphicsObject(PPortContext portContext)
{
    auto canvasScene = getCanvasScene();
    if (!dataPtr->sourceProxy || !portContext || !canvasScene) {
        return nullptr;
    }
    // 图形添加
    QString moduleType = dataPtr->sourceProxy->moduleType();
    QSharedPointer<ICanvasGraphicsObjectFactory> factory = GraphicsFactoryManager::getInstance().getFactory(moduleType);
    if (!factory) {
        return nullptr;
    }

    PortGraphicsObject *portGraphics = factory->createPortGraphicsObject(canvasScene, this, portContext);
    if (!portGraphics) {
        return nullptr;
    }
    portGraphics->setParentItem(getTransformProxyGraphicsObject());
    portGraphics->updatePosition();
    portGraphics->createInternalGraphics();

    // 数据添加
    dataPtr->sourceProxy->addPortContext(portContext);

    return portGraphics;
}

void SourceGraphicsObject::deletePortGraphicsObject(PortGraphicsObject *pPortGraphicsObject)
{
    if (!dataPtr->sourceProxy || !pPortGraphicsObject) {
        return;
    }
    if (!dataPtr->sourceProxy->isSupportAddOrDelPort())
        return;

    deletePortGraphics(pPortGraphicsObject);
}

void SourceGraphicsObject::deletePortGraphics(PortGraphicsObject *pPortGraphicsObject)
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene || !pPortGraphicsObject) {
        return;
    }
    // 把图元从场景中删除
    pPortGraphicsObject->cleanLayerInfo();
    canvasScene->removeItem(pPortGraphicsObject);
    pPortGraphicsObject->deleteLater();
}

QRectF SourceGraphicsObject::getSourceSceneRect()
{
    return mapToScene(dataPtr->sourceBoundingRect).boundingRect();
}
