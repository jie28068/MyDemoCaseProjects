#include "AnnotationSourceGraphicsObject.h"
#include "GeometricGraphicsAnnotation.h"
#include "ICanvasView.h"
#include "SourceProxy.h"
#include "TransformProxyGraphicsObject.h"

#include <QDebug>
#include <QMouseEvent>

AnnotationSourceGraphicsObject::AnnotationSourceGraphicsObject(ICanvasScene *canvasScene,
                                                               QSharedPointer<SourceProxy> source,
                                                               QGraphicsItem *parent)
    : SourceGraphicsObject(canvasScene, source, parent)
{
    status = kStatusStarted;
    setXAxisAlignment(kAlignmentNone);
    setYAxisAlignment(kAlignmentNone);
    onRefreshAnnotationType();
}

AnnotationSourceGraphicsObject::~AnnotationSourceGraphicsObject() { }

void AnnotationSourceGraphicsObject::processMousePressEvent(QMouseEvent *event) { }

void AnnotationSourceGraphicsObject::processMouseMoveEvent(QMouseEvent *event) { }

void AnnotationSourceGraphicsObject::processMouseReleaseEvent(QMouseEvent *event) { }

void AnnotationSourceGraphicsObject::processMouseDoubleClickEvent(QMouseEvent *event) { }

void AnnotationSourceGraphicsObject::setMovingPos(const QPointF &scenePos) { }

void AnnotationSourceGraphicsObject::createInternalGraphics() { }

AnnotationSourceGraphicsObject::AnnotationType AnnotationSourceGraphicsObject::getAnnotationType() const
{
    return annotationType;
}

void AnnotationSourceGraphicsObject::setStatus(Status s)
{
    status = s;
    if (s & kStatusCancled) {
        emit cancled();
    } else if (s & kStatusFinished) {
        emit interactionFinished();
    } else {
    }
}

bool AnnotationSourceGraphicsObject::isInterfactionFinished()
{
    return status & kStatusFinished;
}

void AnnotationSourceGraphicsObject::userShow(bool visible)
{
    SourceGraphicsObject::userShow(visible);
    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (proxyGraphics) {
        proxyGraphics->userShow(visible);
    }
}

void AnnotationSourceGraphicsObject::onRefreshAnnotationType()
{
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    QString prototypeName = sourceProxy->prototypeName();
    if (prototypeName.compare(CircleAnnotationStr) == 0) {
        annotationType = kCircleAnnotation;
    } else if (prototypeName.compare(RectAnnotationStr) == 0) {
        annotationType = kRectAnnotation;
    } else if (prototypeName.compare(LineSegmentAnnotationStr) == 0) {
        annotationType = kLineSegment;
    } else if (prototypeName.compare(LineSegmentArrowAnnotationStr) == 0) {
        annotationType = kLineSegmentArrow;
    } else if (prototypeName.compare(BrokenLineAnnotationStr) == 0) {
        annotationType = kBrokenLineAnnotation;
    } else if (prototypeName.compare(BrokenLineArrowAnnotationStr) == 0) {
        annotationType = kBrokenLineArrowAnnotation;
    } else if (prototypeName.compare(PolygonAnnotationStr) == 0) {
        annotationType = kPolygonAnnotation;
    } else if (prototypeName.compare(ArcAnnotationStr) == 0) {
        annotationType = kArcAnnotation;
    } else {
        annotationType = kNoneAnnotation;
    }
}

void AnnotationSourceGraphicsObject::setAnnotationPoints(QPolygonF points)
{
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    sourceProxy->setAnnotationProperty(Annotation::pointArray, QVariant::fromValue(points.toPolygon()));
}

QPolygon AnnotationSourceGraphicsObject::getAnnotationPoints()
{
    QPolygon points;
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return points;
    }

    QVariant value = sourceProxy->getAnnotationMap().value(Annotation::pointArray, QVariant());
    if (value.canConvert<QPolygon>()) {
        points = value.value<QPolygon>();
    }
    return points;
}

double AnnotationSourceGraphicsObject::getAnnotationLineWidth() const
{
    double width = 3;
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return width;
    }
    QVariant value = sourceProxy->getAnnotationMap().value(Annotation::segmentWidth, QVariant());
    if (value.canConvert<double>()) {
        width = value.value<double>();
    }
    return width;
}

void AnnotationSourceGraphicsObject::setAnnotationLineWidth(double width)
{
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    width = width < 1.0 ? 1 : width;
    width = width > 10 ? 10 : width;
    sourceProxy->getAnnotationMap().insert(Annotation::segmentWidth, QVariant::fromValue(width));
}

QPointF AnnotationSourceGraphicsObject::getMouseEventScenePos(QMouseEvent *event)
{
    auto canvasScene = getCanvasScene();
    if (!canvasScene) {
        return QPointF();
    }
    QList<QGraphicsView *> views = canvasScene->views();
    for each (auto view in views) {
        ICanvasView *canvasView = dynamic_cast<ICanvasView *>(view);
        if (canvasView) {
            QPointF pos = event->pos();
            QPointF scenePos = canvasView->mapToScene(pos.toPoint());
            return scenePos;
        }
    }
    return QPointF();
}

Qt::PenStyle AnnotationSourceGraphicsObject::getSegmentStyle() const
{
    Qt::PenStyle style = Qt::SolidLine;
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return style;
    }
    QVariant value = sourceProxy->getAnnotationMap().value(Annotation::segmentStyle, QVariant());
    if (value.canConvert<int>()) {
        style = (Qt::PenStyle)value.value<int>();
    }
    return style;
}

bool AnnotationSourceGraphicsObject::getShowFrontArrow() const
{
    bool getShowFrontArrow = false;
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return getShowFrontArrow;
    }
    QVariant value = sourceProxy->getAnnotationMap().value(Annotation::isShowHeadArrow, QVariant());
    if (value.canConvert<bool>()) {
        getShowFrontArrow = value.value<bool>();
    }
    return getShowFrontArrow;
}

bool AnnotationSourceGraphicsObject::getShowEndArrow() const
{
    bool getShowEndArrow = false;
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return getShowEndArrow;
    }
    QVariant value = sourceProxy->getAnnotationMap().value(Annotation::isShowArrow, QVariant());
    if (value.canConvert<bool>()) {
        getShowEndArrow = value.value<bool>();
    }
    return getShowEndArrow;
}

QString AnnotationSourceGraphicsObject::getContent() const
{
    QString content = QString();
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return "";
    }
    QVariant value = sourceProxy->getAnnotationMap().value(Annotation::content, QVariant());
    if (value.canConvert<QString>()) {
        content = value.value<QString>();
    }
    return content;
}

QFont AnnotationSourceGraphicsObject::getFont() const
{
    QFont font = QFont();
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return font;
    }
    QVariant value = sourceProxy->getAnnotationMap().value(Annotation::textFont, QVariant());
    if (value.canConvert<QFont>()) {
        font = value.value<QFont>();
    }
    return font;
}

QColor AnnotationSourceGraphicsObject::getSegmentColor() const
{
    QColor color = QColor(GKD::WIRE_NORMAL_COLOR);
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return color;
    }
    QVariant value = sourceProxy->getAnnotationMap().value(Annotation::segmentColor, QVariant());
    if (value.canConvert<QColor>()) {
        color = value.value<QColor>();
    }
    return color;
}

void AnnotationSourceGraphicsObject::initProperty() { }
