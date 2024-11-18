#include "SizeControlGraphicsObject.h"
#include "CanvasContext.h"
#include "ICanvasScene.h"
#include "SizeControlGraphicsObjectPrivate.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"

#include <QCursor>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

const qreal controlSize = 5.0;

SizeControlGraphicsObject::SizeControlGraphicsObject(ICanvasScene *canvasScene,
                                                     SizeControlGraphicsObject::Direction direction,
                                                     QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent)
{
    dataPtr.reset(new SizeControlGraphicsObjectPrivate());
    dataPtr->direction = direction;
    setAcceptHoverEvents(true);
    setLayerFlag(GraphicsLayerFlag::kSizeControlGraphicsLayer);
}

SizeControlGraphicsObject::~SizeControlGraphicsObject() { }

QString SizeControlGraphicsObject::id()
{
    return dataPtr->id;
}

SizeControlGraphicsObject::Direction SizeControlGraphicsObject::getDirection()
{
    return dataPtr->direction;
    TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(parentItem());
    if (proxyGraphics) {
        int angle = proxyGraphics->getAngle();
        if (angle < 0) {
            angle = 360 - angle;
        }
        int offset = angle / 90;
        SizeControlGraphicsObject::Direction direction =
                SizeControlGraphicsObject::Direction(dataPtr->direction + offset * 2);
        if (direction > SizeControlGraphicsObject::kDirectionLeftCenter) {
            direction = SizeControlGraphicsObject::Direction(int(direction)
                                                             % int(SizeControlGraphicsObject::kDirectionLeftCenter));
        }
        return direction;
    }
    return dataPtr->direction;
}

Qt::CursorShape SizeControlGraphicsObject::getCursorShape()
{
    switch (getDirection()) {
    case kDirectionRightCenter:
        return Qt::SizeHorCursor;
    case kDirectionRightTop:
        return Qt::SizeBDiagCursor;
    case kDirectionRightBottom:
        return Qt::SizeFDiagCursor;
    case kDirectionLeftBottom:
        return Qt::SizeBDiagCursor;
    case kDirectionBottomCenter:
        return Qt::SizeVerCursor;
    case kDirectionLeftTop:
        return Qt::SizeFDiagCursor;
    case kDirectionLeftCenter:
        return Qt::SizeHorCursor;
    case kDirectionTopCenter:
        return Qt::SizeVerCursor;
        break;
    default: {
        return Qt::ArrowCursor;
    }
    }
    return Qt::ArrowCursor;
}

void SizeControlGraphicsObject::userShow(bool visible)
{
    ICanvasScene *scene = getCanvasScene();
    auto canvasContext = getCanvasContext();
    if (scene && canvasContext) {
        bool canModify = canvasContext->canModify();
        if (!canModify) {
            setVisible(false);
            return;
        }
        if (getTransformProxyGraphicsObject() != topLevelItem()) {
            setVisible(false);
            return;
        }
    }
    GraphicsLayer::userShow(visible);
}

QRectF SizeControlGraphicsObject::boundingRect() const
{
    return QRectF(-controlSize / 2, -controlSize / 2, controlSize, controlSize);
}

void SizeControlGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, false);
    QPen pen(QColor("#00cfff"), 1);
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->setBrush(dataPtr->brushColor);
    painter->drawRect(boundingRect());
    painter->restore();
}

void SizeControlGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    auto canvasScene = getCanvasScene();
    if (canvasScene) {
        canvasScene->resetSubsystemGraphics();
    }
    if (event->button() != Qt::LeftButton) {
        return GraphicsLayer::mousePressEvent(event);
    }
    dataPtr->pressPos = event->scenePos();
    event->accept();
    emit readyChange();
}

void SizeControlGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        return GraphicsLayer::mouseMoveEvent(event);
    }
    QPointF offset = event->lastScenePos() - dataPtr->pressPos;
    switch (dataPtr->direction) {
    case kDirectionInvalid: {
        return GraphicsLayer::mouseMoveEvent(event);
    } break;
    case kDirectionRightCenter:
    case kDirectionLeftCenter: {
        offset.setY(0);
        if (offset != QPointF(0, 0)) {
            emit sizeChanged(offset);
        }

    } break;
    case kDirectionTopCenter:
    case kDirectionBottomCenter: {
        offset.setX(0);
        if (offset != QPointF(0, 0)) {
            emit sizeChanged(offset);
        }
    } break;
    default: {
        if (offset != QPointF(0, 0)) {
            emit sizeChanged(offset);
        }
    }
    }
    event->accept();
}

void SizeControlGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    emit finishChange();
    return GraphicsLayer::mouseReleaseEvent(event);
}

QList<SizeControlGraphicsObject *> SizeControlGraphicsObject::createAllSizeControlGraphics(ICanvasScene *canvasScene)
{
    QList<SizeControlGraphicsObject *> result;
    if (nullptr == canvasScene) {
        return result;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return result;
    }
    for (int index = SizeControlGraphicsObject::kDirectionInvalid + 1;
         index <= SizeControlGraphicsObject::kDirectionLeftCenter; index++) {
        // 不创建四周中点的拖拽图元
        if ((index == kDirectionTopCenter || index == kDirectionRightCenter || index == kDirectionBottomCenter
             || index == kDirectionLeftCenter) && canvasContext->type() != CanvasContext::kElectricalType && canvasContext->type() != CanvasContext::kElecUserDefinedType) {
            continue;
        }
        SizeControlGraphicsObject *graphics =
                new SizeControlGraphicsObject(canvasScene, SizeControlGraphicsObject::Direction(index));
        if (graphics) {
            result.append(graphics);
            graphics->userShow(false);
        }
    }
    return result;
}

void SizeControlGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(getCursorShape());
    dataPtr->brushColor = QColor(Qt::blue);
    event->accept();
    GraphicsLayer::hoverEnterEvent(event);
}

void SizeControlGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::ArrowCursor);
    dataPtr->brushColor = QColor(Qt::white);
    event->accept();
    GraphicsLayer::hoverLeaveEvent(event);
}
