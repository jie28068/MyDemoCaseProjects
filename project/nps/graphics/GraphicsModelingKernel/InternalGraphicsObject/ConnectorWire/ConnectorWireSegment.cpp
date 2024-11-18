#include "ConnectorWireSegment.h"
#include "ActionManager.h"
#include "CanvasContext.h"
#include "ConnectorWireCommand.h"
#include "ConnectorWireGraphicsObject.h"
#include "ICanvasScene.h"
#include "PortGraphicsObject.h"
#include "PortPositionCommand.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"

#include <QCursor>
#include <QPainter>
#include <qmath.h>

ConnectorWireSegment::ConnectorWireSegment(ICanvasScene *canvasScene, ConnectorWireGraphicsObject *connectorWire)
    : GraphicsLayer(canvasScene, (QGraphicsItem *)connectorWire)
{
    attachWire = connectorWire;
    segmentType = kSegmentCommon;
    setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);

    connect(this, SIGNAL(positionChanged(QPointF)), connectorWire, SLOT(onSegmentPositionChanged(QPointF)));
    connect(this, &ConnectorWireSegment::selectedChanged, connectorWire,
            &ConnectorWireGraphicsObject::onSegmentSelectedChanged);
}

ConnectorWireSegment::~ConnectorWireSegment() { }

ConnectorWireSegment::SegmentType ConnectorWireSegment::getSegmentType()
{
    return segmentType;
}

void ConnectorWireSegment::setSegmentType(SegmentType type)
{
    segmentType = type;
}

void ConnectorWireSegment::setPoints(const QPoint &startpt, const QPoint &endpt)
{
    startPos = startpt;
    endPos = endpt;
    resetDirection();
}

bool ConnectorWireSegment::isVertical()
{
    return direction == kBottomToTop || direction == kTopToBottom;
}

QPoint ConnectorWireSegment::getStartPoint()
{
    return startPos;
}

QPoint ConnectorWireSegment::getEndPoint()
{
    return endPos;
}

ConnectorWireSegment::SegmentDirection ConnectorWireSegment::getSegmentDirection()
{
    return direction;
}

bool ConnectorWireSegment::isOverLapWithOtherSegment()
{
    QList<QGraphicsItem *> items = collidingItems();
    for each (auto item in items) {
        if (item->type() == kConnectorWireSegment && item->parentItem() != parentItem()) {
            ConnectorWireSegment *segment = dynamic_cast<ConnectorWireSegment *>(item);
            if (!segment) {
                continue;
            }
            if (segment->isVertical() == isVertical()) {
                return true;
            }
        }
    }
    return false;
}

int ConnectorWireSegment::type() const
{
    return kConnectorWireSegment;
}

void ConnectorWireSegment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!attachWire) {
        return;
    }
    QSharedPointer<ConnectorWireContext> context = attachWire->getConnectorWireContext();
    if (!context) {
        return;
    }

    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return;
    }

    if (startPos == endPos) {
        return;
    }

    bool isVisiblea = isVisible();
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, false);
    if (attachWire->hasSegmentSelected()) {
        QColor penColor("#00cfff");
        penColor.setAlpha(200);
        QPen pen(penColor, 2);
        pen.setJoinStyle(Qt::MiterJoin);
        pen.setMiterLimit(0);
        // pen.setCosmetic(true);
        painter->setPen(pen);
        painter->drawLine(startPos, endPos);
    }

    QColor lineColor = canvasContext->lineColor();
    Qt::PenStyle style = (Qt::PenStyle)canvasContext->lineStyle();
    auto srcSource = context->srcSource();
    auto dstSource = context->dstSource();
    if (srcSource && srcSource->state() != "normal" && dstSource && dstSource->state() != "normal") {
        lineColor = QColor(GKD::WIRE_DISABLE_COLOR);
    }

    // 如果连接线连接的端口有一个是被注释为禁用的，那连接线置灰
    if ((srcSource && srcSource->getCommentState() == "disable")
        || (dstSource && dstSource->getCommentState() == "disable")) {
        lineColor = QColor(GKD::WIRE_DISABLE_COLOR);
    }

    // 如果连接线断开了
    if (!context->connected()) {
        lineColor = QColor(GKD::WIRE_DISABLE_COLOR);
    }

    if (attachWire->hasSegmentSelected()) {
        lineColor = canvasContext->lineSelectColor();
    }

    if (attachWire->isConnectedWithPort()) {
        if (attachWire->isHighLightHomologous()) {
            lineColor = canvasContext->lineSelectColor();
        }
    } else {
        style = Qt::DotLine;
    }

    QPen pen(lineColor, 1, style);
    if (canvasContext->type() != CanvasContext::kElectricalType
        && canvasContext->type() != CanvasContext::kElecUserDefinedType) {
        pen.setCosmetic(true);
    } else {
        if (attachWire && !attachWire->isConnectedWithPort()) {
            pen.setCosmetic(true);
        } else {
            painter->setRenderHint(QPainter::Antialiasing, true);
        }
    }
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setMiterLimit(1);
    painter->setPen(pen);

    painter->drawLine(startPos, endPos);

    if (segmentType == kSegmentEnd && canvasContext->showArrow()) {
        painter->setRenderHint(QPainter::Antialiasing, true);
        int length = 8;
        qreal angle = std::atan2(endPos.y() - startPos.y(), endPos.x() - startPos.x());
        if (startPos.y() != endPos.y()) {
            angle = -angle;
        }

        QPointF arrowP1 = endPos - QPointF(sin(angle + M_PI / 3) * length, cos(angle + M_PI / 3) * length);
        QPointF arrowP2 =
                endPos - QPointF(sin(angle + M_PI - M_PI / 3) * length, cos(angle + M_PI - M_PI / 3) * length);
        QPolygonF arrowPoints;
        arrowPoints.push_back(endPos);
        arrowPoints.push_back(arrowP1);
        arrowPoints.push_back(arrowP2);
        QPainterPath arrowPath;
        arrowPath.addPolygon(arrowPoints);
        arrowPath.closeSubpath();
        painter->fillPath(arrowPath, QBrush(lineColor));
    }

    painter->restore();
}

QVariant ConnectorWireSegment::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case QGraphicsItem::ItemPositionChange: {
        if (!canMove()) {
            return pos();
        }

        QPointF alignmentPos = value.toPointF();
        QPointF nowPos = pos();
        if (isVertical()) {
            alignmentPos.setY(nowPos.y());
        } else {
            alignmentPos.setX(nowPos.x());
        }
        alignmentPos = Utility::pointAlignmentToGrid(alignmentPos, 10);
        return alignmentPos;
        break;
    } break;
    case QGraphicsItem::ItemPositionHasChanged: {
        QPointF nowPos = pos();
        emit positionChanged(nowPos);
    } break;
    case QGraphicsItem::ItemSelectedHasChanged: {
        bool isSelected = value.toBool();
        emit selectedChanged(isSelected);
    } break;

    default:
        break;
    }
    return GraphicsLayer::itemChange(change, value);
}

QPainterPath ConnectorWireSegment::shape() const
{
    QPainterPath path;
    path.moveTo(startPos);
    path.lineTo(endPos);
    QPainterPathStroker stroker;
    stroker.setWidth(4);
    path = stroker.createStroke(path);
    return path;
}

QRectF ConnectorWireSegment::boundingRect() const
{
    return shape().controlPointRect();
}

void ConnectorWireSegment::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    GraphicsLayer::mouseMoveEvent(event);
}

void ConnectorWireSegment::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return GraphicsLayer::mousePressEvent(event);
    }
    pressedPos = pos().toPoint();
    // 画板非冻结模式下，线段才可以拖动
    bool canModify = canvasContext->canModify();
    if (canMove() && canModify) {
        setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
        oldPortContext.clear();
        PortGraphicsObject *movingPortGraphics = nullptr;
        if (segmentType == kSegmentCommon) {

            oldWireContext = QSharedPointer<ConnectorWireContext>(
                    new ConnectorWireContext(*attachWire->getConnectorWireContext()));
        } else if (segmentType == kSegmentFirst) {
            movingPortGraphics = attachWire->getOutputTypePortGraphics();
        } else if (segmentType == kSegmentEnd) {
            movingPortGraphics = attachWire->getInputTypePortGraphics();
        }
        if (movingPortGraphics) {
            oldPortContext = QSharedPointer<PortContext>(new PortContext(*movingPortGraphics->getPortContext()));
            auto wires = movingPortGraphics->getLinkedConnectorWireList();
            for each (auto wire in wires) {
                if (wire) {
                    auto wireCtx = wire->getConnectorWireContext();
                    if (wireCtx) {
                        oldWireContextList.push_back(
                                QSharedPointer<ConnectorWireContext>(new ConnectorWireContext(*wireCtx)));
                    }
                }
            }
        }
    }
    GraphicsLayer::mousePressEvent(event);
}

void ConnectorWireSegment::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return GraphicsLayer::mouseReleaseEvent(event);
    }
    if (!attachWire) {
        return;
    }
    auto undoStack = canvasScene->getUndoStack();
    QPoint nowPos = pos().toPoint();
    if (nowPos != pressedPos) {
        startPos += nowPos;
        endPos += nowPos;
        setPos(QPointF(0, 0));

        PortGraphicsObject *movingPortGraphics = nullptr;

        if (segmentType == kSegmentCommon) {
            auto currentWireContext = QSharedPointer<ConnectorWireContext>(
                    new ConnectorWireContext(*attachWire->getConnectorWireContext()));
            ConnectorWireAdjustCommand *command =
                    new ConnectorWireAdjustCommand(canvasScene, attachWire->id(), oldWireContext, currentWireContext);
            if (undoStack) {
                undoStack->push(command);
            }
        } else if (segmentType == kSegmentFirst) {
            movingPortGraphics = attachWire->getOutputTypePortGraphics();
        } else if (segmentType == kSegmentEnd) {
            movingPortGraphics = attachWire->getInputTypePortGraphics();
        }
        if (movingPortGraphics) {
            auto currentPortContext =
                    QSharedPointer<PortContext>(new PortContext(*movingPortGraphics->getPortContext()));
            QList<PConnectorWireContext> currentWireContextList;
            auto wires = movingPortGraphics->getLinkedConnectorWireList();
            for each (auto wire in wires) {
                if (wire) {
                    auto wireCtx = wire->getConnectorWireContext();
                    if (wireCtx) {
                        currentWireContextList.push_back(
                                QSharedPointer<ConnectorWireContext>(new ConnectorWireContext(*wireCtx)));
                    }
                }
            }
            auto movingProxyGraphics = movingPortGraphics->getTransformProxyGraphicsObject();
            if (oldPortContext && currentPortContext && movingProxyGraphics) {
                QString proxyGraphicsID = movingProxyGraphics->id();
                PortPositionCommand *command =
                        new PortPositionCommand(canvasScene, proxyGraphicsID, oldPortContext, currentPortContext,
                                                oldWireContextList, currentWireContextList);
                if (undoStack) {
                    undoStack->push(command);
                }
            }
        }
    }
    if (canMove()) {
        setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
    }
    GraphicsLayer::mouseReleaseEvent(event);
}

void ConnectorWireSegment::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (attachWire) {
        attachWire->mouseDoubleClickEvent(event);
    } else {
        GraphicsLayer::mouseDoubleClickEvent(event);
    }
}

void ConnectorWireSegment::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (canMove()) {
        if (isVertical()) {
            setCursor(QCursor(Qt::SizeHorCursor));
        } else {
            setCursor(QCursor(Qt::SizeVerCursor));
        }
    }

    GraphicsLayer::hoverEnterEvent(event);
}

void ConnectorWireSegment::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    unsetCursor();
    GraphicsLayer::hoverLeaveEvent(event);
}

void ConnectorWireSegment::resetDirection()
{
    if (startPos == endPos) {
        direction = kNone;
    } else {
        if (startPos.x() == endPos.x()) {
            if (startPos.y() <= endPos.y()) {
                direction = kTopToBottom;
            } else {
                direction = kBottomToTop;
            }
        } else if (startPos.y() == endPos.y()) {
            if (startPos.x() <= endPos.x()) {
                direction = kLeftToRight;
            } else {
                direction = kRightToLeft;
            }
        }
    }
}

bool ConnectorWireSegment::canMove()
{
    if (!attachWire) {
        return false;
    }
    if (!attachWire->isConnectedWithPort()) {
        return false;
    }
    auto intputPortGraphics = attachWire->getInputTypePortGraphics();
    auto outputPortGraphics = attachWire->getOutputTypePortGraphics();
    bool ret = false;
    switch (segmentType) {
    case kSegmentCommon:
        ret = true;
        break;
    case kSegmentFirst:
        if (outputPortGraphics) {
            ret = outputPortGraphics->canMove();
        }

        break;
    case kSegmentEnd:
        if (intputPortGraphics) {
            ret = intputPortGraphics->canMove();
        }

        break;
    default: {
    }
    }
    return ret;
}

int ConnectorWireSegment::getDistance()
{
    return (startPos - endPos).manhattanLength();
}
