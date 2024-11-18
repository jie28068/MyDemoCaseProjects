#include "AnnotationPoint.h"
#include "CanvasContext.h"
#include "GeometricGraphicsAnnotation.h"
#include "ICanvasScene.h"
#include "SourceProxy.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"

#include <QCursor>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

const qreal controlSize = 10.0;

AnnotationPoint::AnnotationPoint(ICanvasScene *canvasScene, int index, QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent), pointIndex(index)
{
    // setFlags(ItemIsMovable | ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setLayerFlag(GraphicsLayerFlag::kCommentPoint);
}

AnnotationPoint::~AnnotationPoint() { }

QList<AnnotationPoint *> AnnotationPoint::createAllAnnotationGraphics(ICanvasScene *canvasScene, QPolygonF &cachePoints)
{
    QList<AnnotationPoint *> result;
    if (nullptr == canvasScene) {
        return result;
    }
    for (int i = 0; i < cachePoints.size(); ++i) {
        AnnotationPoint *graphics = new AnnotationPoint(canvasScene, i);
        if (graphics) {
            result.append(graphics);
            graphics->userShow(false);
        }
    }
    return result;
}

QRectF AnnotationPoint::boundingRect() const
{
    return QRectF(-controlSize / 2, -controlSize / 2, controlSize, controlSize);
}

// void AnnotationPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
//{
//     /*painter->save();
//     painter->setPen(Qt::SolidLine);
//     painter->setBrush(QColor(0x2E9FE6));
//     painter->setRenderHint(QPainter::Antialiasing);
//     painter->drawEllipse(boundingRect());
//     painter->restore();*/
// }

void AnnotationPoint::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return GraphicsLayer::mousePressEvent(event);
    }
    event->accept();
    pressPoint = event->scenePos();
    emit positionChanged(QPointF(), pointIndex, startedChange);
}

void AnnotationPoint::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        return GraphicsLayer::mouseMoveEvent(event);
    }
    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (!proxyGraphics) {
        return GraphicsLayer::mouseMoveEvent(event);
    }
    QTransform trans;
    qreal angle = proxyGraphics->getAngle();
    trans.rotate(-angle);
    QPointF startP = pressPoint;
    startP = trans.map(startP);
    QPointF offset = trans.map(event->scenePos()) - startP;
    event->accept();
    emit positionChanged(offset, pointIndex, inChange);
}

void AnnotationPoint::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return GraphicsLayer::mouseReleaseEvent(event);
    }
    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (!proxyGraphics) {
        return GraphicsLayer::mouseReleaseEvent(event);
    }
    QTransform trans;
    qreal angle = proxyGraphics->getAngle();
    trans.rotate(-angle);
    QPointF startP = pressPoint;
    startP = trans.map(startP);
    QPointF offset = trans.map(event->scenePos()) - startP;
    event->accept();
    emit positionChanged(offset, pointIndex, finishedChange);
}
