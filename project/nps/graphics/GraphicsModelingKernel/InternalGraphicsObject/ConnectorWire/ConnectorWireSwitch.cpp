#include "ConnectorWireSwitch.h"
#include "CanvasContext.h"
#include "ConnectorWireCommand.h"
#include "ConnectorWireContext.h"
#include "ConnectorWireGraphicsObject.h"
#include "GraphicsKernelDefinition.h"
#include "ICanvasScene.h"
#include "SourceProxy.h"

#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

ConnectorWireSwitch::ConnectorWireSwitch(ICanvasScene *canvasScene, bool status, QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent)
{
    connectorWireGraphicsObject = nullptr;
    isOn = status;
    setAcceptHoverEvents(true);
    setLayerFlag(GraphicsLayer::kConnectorWireGraphicsLayer);
}

ConnectorWireSwitch::~ConnectorWireSwitch()
{
    disconnect();
}

int ConnectorWireSwitch::type() const
{
    return kConnectorWireSwitch;
}

void ConnectorWireSwitch::attachConnectorWire(ConnectorWireGraphicsObject *wire)
{
    if (!wire) {
        return;
    }
    connectorWireGraphicsObject = wire;
    PConnectorWireContext context = connectorWireGraphicsObject->getConnectorWireContext();
    if (context) {
        connect(context.data(), &ConnectorWireContext::connectedChanged, this,
                &ConnectorWireSwitch::onConnectStatusChanged);
    }
}

void ConnectorWireSwitch::detachConnectorWire()
{
    connectorWireGraphicsObject = nullptr;
}

void ConnectorWireSwitch::doSwitch(bool isSwitchOn)
{
    if (isOn == isSwitchOn) {
        return;
    }
    isOn = isSwitchOn;
    update();
    emit switchStatusChanged(isOn);
}

QRectF ConnectorWireSwitch::boundingRect() const
{
    int gridSpace = 10;
    return QRectF(-(gridSpace / 4.f), -(gridSpace / 4.f), gridSpace / 2.f, gridSpace / 2.f);
}

void ConnectorWireSwitch::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */)
{
    if (nullptr == connectorWireGraphicsObject) {
        return;
    }
    QColor penColor(GKD::WIRE_NORMAL_COLOR);
    // 如果连接线的资源状态不都是normal，需要变更开关颜色
    PConnectorWireContext context = connectorWireGraphicsObject->getConnectorWireContext();
    if (!context) {
        return;
    }

    auto canvasContext = getCanvasContext();
    if (canvasContext && canvasContext->isShowPowerFlowData()) {
        // 显示潮流计算结果时，开关不显示，否则会把潮流计算结果给遮住
        return;
    }

    auto srcSource = context->srcSource();
    auto dstSource = context->dstSource();
    if (srcSource && srcSource->state() != "normal" && dstSource && dstSource->state() != "normal") {
        penColor = QColor(GKD::WIRE_DISABLE_COLOR);
    }

    painter->save();
    if (isOn) {
        painter->setPen(QPen(penColor, 1, Qt::SolidLine));
        painter->setBrush(QBrush(penColor, Qt::SolidPattern));
        painter->drawRect(boundingRect());
    } else {
        penColor = QColor(GKD::WIRE_DISABLE_COLOR);
        QColor backgroundColor(Qt::white);
        auto canvasContext = getCanvasContext();
        if (canvasContext) {
            backgroundColor = canvasContext->backgroundColor();
        }
        painter->setPen(QPen(penColor, 1, Qt::SolidLine));
        painter->setBrush(QBrush(backgroundColor, Qt::SolidPattern));
        painter->drawRect(boundingRect());
    }
    painter->restore();
}

void ConnectorWireSwitch::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::PointingHandCursor);
    update();
    GraphicsLayer::hoverLeaveEvent(event);
}

void ConnectorWireSwitch::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::ArrowCursor);
    update();
    GraphicsLayer::hoverLeaveEvent(event);
}

void ConnectorWireSwitch::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return;
    }
    auto canvasCtx = canvasScene->getCanvasContext();
    if (canvasCtx && !canvasCtx->canModify()) {
        return;
    }

    if (!connectorWireGraphicsObject) {
        return;
    }

    PConnectorWireContext ctx = connectorWireGraphicsObject->getConnectorWireContext();
    if (ctx) {
        ConnectorWireSwitchCommand *command = new ConnectorWireSwitchCommand(getCanvasScene(), ctx, !isOn);
        auto undoStack = canvasScene->getUndoStack();
        if (undoStack) {
            undoStack->push(command);
        }
    }
}

void ConnectorWireSwitch::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // event->accept();
}

void ConnectorWireSwitch::onConnectStatusChanged(bool connected)
{
    doSwitch(connected);
}