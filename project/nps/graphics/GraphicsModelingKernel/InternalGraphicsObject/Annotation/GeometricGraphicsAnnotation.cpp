#include "GeometricGraphicsAnnotation.h"
#include "GraphicsKernelDefinition.h"
#include "SourceProxy.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <qmath.h>

const qreal controlSize = 5.0;

GeometricGraphicsAnnotation::GeometricGraphicsAnnotation(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                                         QGraphicsItem *parent)
    : AnnotationSourceGraphicsObject(canvasScene, source, parent)
{
    setScaleDirection(kScaleNone);
    initProperty();
    cachePoints = getAnnotationPoints();
    if (!cachePoints.isEmpty()) {
        completePoint(cachePoints);
        status = kStatusFinished;
    }
    movingPoint = QPointF();
    setLayerFlag(kCommentGraphicsLayer);
}

GeometricGraphicsAnnotation::~GeometricGraphicsAnnotation() { }

int GeometricGraphicsAnnotation::type() const
{
    return kGeometricAnnotationGraphics;
}

void GeometricGraphicsAnnotation::processMousePressEvent(QMouseEvent *event)
{
    if (!(status & kStatusStarted)) {
        return;
    }
    if(event->button() == Qt::RightButton)
    {
        setStatus(kStatusCancled);
        return;
    }
    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (!proxyGraphics) {
        return;
    }
    proxyGraphics->setFlag(ItemIsSelectable, false);
    QPointF scenePos = getMouseEventScenePos(event);
    QPointF alignPos = Utility::pointAlignmentToGrid(scenePos, 10);
    switch (annotationType) {
    case kCircleAnnotation:
    case kRectAnnotation:
        if (cachePoints.isEmpty()) {
            cachePoints.push_back(alignPos);
            cachePoints.push_back(alignPos);
        }
        break;
    case kLineSegment:
    case kLineSegmentArrow:
        if (event->button() == Qt::RightButton) {
            // 鼠标右键
            setStatus(kStatusCancled);
            return;
        }
        if (cachePoints.isEmpty()) {
            cachePoints.push_back(alignPos);
        }
        if (cachePoints.size() >= 2) {
            cachePoints[1] = alignPos;
            interactionFinish();
        }
        break;
    case kBrokenLineAnnotation:
    case kBrokenLineArrowAnnotation:
    case kPolygonAnnotation:
        if (event->button() == Qt::RightButton) {
            // 鼠标右键
            setStatus(kStatusCancled);
            return;
        }
        if (cachePoints.isEmpty()) {
            cachePoints.push_back(alignPos);
            cachePoints.push_back(alignPos);
        } else {
            cachePoints.last() = alignPos;
            cachePoints.push_back(alignPos);
        }

        break;
    case kArcAnnotation: {
        if (cachePoints.isEmpty()) {
            cachePoints.push_back(scenePos);
        } else {
            if (event->button() == Qt::RightButton) {
                // 鼠标右键
                setStatus(kStatusCancled);
                return;
            }
            cachePoints.push_back(scenePos);
            interactionFinish();
        }
    } break;
    default: {
    }
    }
}

void GeometricGraphicsAnnotation::processMouseMoveEvent(QMouseEvent *event)
{
    if (!(status & kStatusStarted) || cachePoints.isEmpty()) {
        return;
    }
    QPointF scenePos = getMouseEventScenePos(event);
    QPointF alignPos = Utility::pointAlignmentToGrid(scenePos, 10);
    movingPoint = scenePos;
    switch (annotationType) {
    case kCircleAnnotation:
    case kRectAnnotation:
        cachePoints.last() = alignPos;
        break;
    case kLineSegment:
    case kLineSegmentArrow:
        if (cachePoints.size() < 2) {
            cachePoints.push_back(alignPos);
        } else {
            cachePoints[1] = alignPos;
        }
        break;
    case kBrokenLineAnnotation:
    case kBrokenLineArrowAnnotation:
    case kPolygonAnnotation:
        if (!cachePoints.isEmpty()) {
            cachePoints.last() = alignPos;
        }
        break;
        /*case kArcAnnotation: {
          if (cachePoints.size() == 1) {
                cachePoints.push_back(alignPos);
          } else if (cachePoints.size() == 2){
                cachePoints.last() = alignPos;
          }

        }*/
        break;
    default: {
    }
    }

    update();
}

void GeometricGraphicsAnnotation::processMouseReleaseEvent(QMouseEvent *event)
{
    if (!(status & kStatusStarted)) {
        return;
    }
    QPointF scenePos = getMouseEventScenePos(event);
    QPointF alignPos = Utility::pointAlignmentToGrid(scenePos, 10);
    switch (annotationType) {
    case kCircleAnnotation:
    case kRectAnnotation:
        cachePoints.last() = alignPos;
        interactionFinish();
        break;
    case kArcAnnotation: {
        if (cachePoints.isEmpty()) {
            setStatus(kStatusCancled);
            return;
        }
        qreal distance = Utility::lineDistance(cachePoints[0].toPoint(), scenePos.toPoint());
        if (distance < 30) {
            setStatus(kStatusCancled);
            return;
        }
        cachePoints.push_back(scenePos);
    } break;
    default: {
    }
    }
}

void GeometricGraphicsAnnotation::processMouseDoubleClickEvent(QMouseEvent *event)
{
    if (!(status & kStatusStarted)) {
        return;
    }
    QPointF scenePos = getMouseEventScenePos(event);
    QPointF alignPos = Utility::pointAlignmentToGrid(scenePos, 10);
    if (annotationType == kBrokenLineAnnotation || annotationType == kBrokenLineArrowAnnotation
        || annotationType == kPolygonAnnotation) {
        // 折线最后一个点重复存储 去掉
        cachePoints.pop_back();
        cachePoints.last() = alignPos;
        interactionFinish();
    }
}

void GeometricGraphicsAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // 如果被取消,则不需要绘制
    if (status & kStatusCancled) {
        return;
    }
    painter->save();
    QPen pen;
    double lineWidth = getAnnotationLineWidth();
    pen.setStyle(getSegmentStyle());
    QColor color = getSegmentColor();
    pen.setWidthF(lineWidth);
    pen.setColor(color);
    if (isHoverStatus() && (status & kStatusFinished)) { // 如果没绘制完 不需要高亮
        pen.setColor(QColor("lightskyblue"));
    }

    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (proxyGraphics && proxyGraphics->isSelected()) {
        pen.setColor(QColor(0x2E9FE6));
    }

    painter->setPen(pen);
    switch (annotationType) {
    case AnnotationSourceGraphicsObject::kCircleAnnotation: {
        QRectF rc = cachePoints.boundingRect();
        painter->drawEllipse(rc);
    } break;
    case AnnotationSourceGraphicsObject::kRectAnnotation:
        if (isInterfactionFinished()) {
            QPainterPath path;
            path.addPolygon(cachePoints);
            path.closeSubpath();
            painter->drawPath(path);
        } else {
            QRectF rc = cachePoints.boundingRect();
            painter->drawRect(rc);
        }
        break;
    case AnnotationSourceGraphicsObject::kLineSegmentArrow:
    case AnnotationSourceGraphicsObject::kLineSegment: {
        if (cachePoints.size() >= 2) {
            QLineF line;
            line.setPoints(cachePoints.first(), cachePoints.last());
            painter->drawLine(line);
            drawFrontArrow(painter, line);
            drawEndArrow(painter, line);
        }
    } break;
    case AnnotationSourceGraphicsObject::kBrokenLineArrowAnnotation:
    case AnnotationSourceGraphicsObject::kBrokenLineAnnotation: {
        int size = cachePoints.size();
        if (size >= 2) {
            painter->drawPolyline(cachePoints);
            QLineF line;
            line.setPoints(cachePoints[0], cachePoints[1]);
            drawFrontArrow(painter, line);
            line.setPoints(cachePoints[size - 2], cachePoints[size - 1]);
            drawEndArrow(painter, line);
        }
    }

    break;
    case AnnotationSourceGraphicsObject::kPolygonAnnotation: {
        if (cachePoints.size() >= 2) {
            if (isInterfactionFinished()) {
                QPainterPath path;
                path.addPolygon(cachePoints);
                if (cachePoints.size() > 2) {
                    path.closeSubpath();
                }

                painter->drawPath(path);
            } else {
                painter->drawPolyline(cachePoints);
            }
        }

    } break;
    case AnnotationSourceGraphicsObject::kArcAnnotation: {
        if (!cachePoints.isEmpty()) {
            if (isInterfactionFinished()) {
                painter->drawPath(getArcPath(cachePoints[0], cachePoints[1], cachePoints[2]));

            } else {
                if (cachePoints.size() == 1 && !movingPoint.isNull()) {
                    painter->setPen(QPen(Qt::blue, 1, Qt::DashLine));
                    QRectF rc = arcCalcRect(cachePoints[0], movingPoint);
                    painter->drawEllipse(rc);
                    painter->drawLine(QLineF(cachePoints[0], movingPoint));
                } else if (cachePoints.size() == 2) {
                    painter->drawPath(getArcPath(cachePoints[0], cachePoints[1], movingPoint));
                }
            }
        }
    } break;
    default:
        break;
    }

    auto canvasContext = getCanvasContext();
    if (proxyGraphics && proxyGraphics->isSelected() && canvasContext && canvasContext->canModify()) {
        drawPointsRect(painter);
    }

    painter->restore();
}

QRectF GeometricGraphicsAnnotation::boundingRect() const
{
    return shape().controlPointRect();
}

QPainterPath GeometricGraphicsAnnotation::shape() const
{
    QPainterPath path;

    QRectF rcFinished = cachePoints.boundingRect();
    if (status & kStatusFinished) {
        // rcFinished = sourceBoundingRect;
    }
    switch (annotationType) {
    case AnnotationSourceGraphicsObject::kCircleAnnotation:
        path.addEllipse(rcFinished);
        break;
    case AnnotationSourceGraphicsObject::kLineSegment:
    case AnnotationSourceGraphicsObject::kBrokenLineAnnotation:
    case AnnotationSourceGraphicsObject::kLineSegmentArrow:
    case AnnotationSourceGraphicsObject::kBrokenLineArrowAnnotation:
        path.addPolygon(cachePoints);
        break;
    case AnnotationSourceGraphicsObject::kPolygonAnnotation:
    case AnnotationSourceGraphicsObject::kRectAnnotation:
        path.addPolygon(cachePoints);
        path.closeSubpath();
        break;
    case AnnotationSourceGraphicsObject::kArcAnnotation: {
        if (cachePoints.size() == 1) {
            QRectF rc = arcCalcRect(cachePoints[0], movingPoint);
            path.addRect(rc);
        } else if (cachePoints.size() == 2) {
            QRectF rc = arcCalcRect(cachePoints[0], cachePoints[1]);
            path.addRect(rc);
        } else if (cachePoints.size() >= 3) {
            QRectF rc = arcCalcRect(cachePoints[0], cachePoints[1]);
            qreal startAngle = Utility::lineAngle(cachePoints[0].toPoint(), cachePoints[1].toPoint());
            qreal endAngle = Utility::lineAngle(cachePoints[0].toPoint(), cachePoints[2].toPoint());
            path.arcMoveTo(rc, -startAngle);
            path.arcTo(rc, -startAngle, (startAngle - endAngle));
        }
    }
    default:
        break;
    }

    double lineWidth = getAnnotationLineWidth();
    QPainterPathStroker stroker;
    if (lineWidth < 1) {
        lineWidth = 1;
    }
    stroker.setWidth(lineWidth);
    path = stroker.createStroke(path);
    return path;
}

void GeometricGraphicsAnnotation::drawFrontArrow(QPainter *painter, QLineF &line)
{
    // m_frontPoints.clear();
    qreal width = getAnnotationLineWidth();
    qreal arrowLength = width * 2 < 8 ? 8 : width * 2; // 箭头长度
    // 求直线的倾斜角
    double angle = atan2(line.p2().y() - line.p1().y(), line.p2().x() - line.p1().x());
    // 因为直线在画板上有宽度 所以真正的终点还得加上宽度
    qreal startX = line.p1().x() - width / 2 * cos(angle);
    qreal startY = line.p1().y() - width / 2 * sin(angle);

    // 求得箭头辅助点 1 的坐标
    qreal subx1 = startX + (arrowLength - 5) * sin(angle);
    qreal suby1 = startY - (arrowLength - 5) * cos(angle);
    // 求得箭头点 1 的坐标
    qreal x1 = startX + arrowLength * sin(angle);
    qreal y1 = startY - arrowLength * cos(angle);
    // 求得箭头点 2 的坐标
    qreal x2 = startX - arrowLength * cos(angle);
    qreal y2 = startY - arrowLength * sin(angle);
    // 求得箭头点 3 的坐标
    qreal x3 = startX - arrowLength * sin(angle);
    qreal y3 = startY + arrowLength * cos(angle);
    // 求得箭头辅助点 2 的坐标
    qreal subx2 = startX - (arrowLength - 5) * sin(angle);
    qreal suby2 = startY + (arrowLength - 5) * cos(angle);

    // 需要画箭头
    if (getShowFrontArrow()) {
        QPolygonF frontPoints;
        frontPoints << QPointF(subx1, suby1) << QPointF(x1, y1) << QPointF(x2, y2) << QPointF(x3, y3)
                    << QPointF(subx2, suby2);
        painter->setBrush(painter->pen().color());
        QPen pen = painter->pen();
        painter->setPen(Qt::NoPen);
        painter->drawPolygon(frontPoints);
        painter->setBrush(Qt::NoBrush);
        painter->setPen(pen);
    } /*else {
        m_frontPoints << QPointF(subx1, suby1) << QPointF(subx2, suby2);
    }*/
}

void GeometricGraphicsAnnotation::drawEndArrow(QPainter *painter, QLineF &line)
{
    // m_endPoints.clear();
    qreal width = getAnnotationLineWidth();
    qreal arrowLength = width * 2 < 8 ? 8 : width * 2; // 箭头长度
    // 求直线的倾斜角
    double angle = atan2(line.p2().y() - line.p1().y(), line.p2().x() - line.p1().x());
    // 因为直线在画板上有宽度 所以真正的终点还得加上宽度
    qreal endX = line.p2().x() + width / 2 * cos(angle);
    qreal endY = line.p2().y() + width / 2 * sin(angle);

    // 求得箭头辅助点 1 的坐标
    qreal subx1 = endX - (arrowLength - 5) * sin(angle);
    qreal suby1 = endY + (arrowLength - 5) * cos(angle);
    // 求得箭头点 1 的坐标
    qreal x1 = endX - arrowLength * sin(angle);
    qreal y1 = endY + arrowLength * cos(angle);
    // 求得箭头点 2 的坐标
    qreal x2 = endX + arrowLength * cos(angle);
    qreal y2 = endY + arrowLength * sin(angle);
    // 求得箭头点 3 的坐标
    qreal x3 = endX + arrowLength * sin(angle);
    qreal y3 = endY - arrowLength * cos(angle);
    // 求得箭头辅助点 2 的坐标
    qreal subx2 = endX + (arrowLength - 5) * sin(angle);
    qreal suby2 = endY - (arrowLength - 5) * cos(angle);

    // 需要画箭头
    if (getShowEndArrow()) {
        QPolygonF endPoints;
        endPoints << QPointF(subx1, suby1) << QPointF(x1, y1) << QPointF(x2, y2) << QPointF(x3, y3)
                  << QPointF(subx2, suby2);
        painter->setBrush(painter->pen().color());
        QPen pen = painter->pen();
        painter->setPen(Qt::NoPen);
        painter->drawPolygon(endPoints);
        painter->setBrush(Qt::NoBrush);
        painter->setPen(pen);
    }
}

void GeometricGraphicsAnnotation::interactionFinish()
{
    if (status & kStatusFinished) {
        return;
    }

    if (!IsVailid()) {
        setStatus(kStatusCancled);
        return;
    }

    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (!proxyGraphics) {
        setStatus(kStatusCancled);
        return;
    }

    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        setStatus(kStatusCancled);
        return;
    }

    QRectF rcFinished = shape().boundingRect();
    QPointF topLeft = rcFinished.topLeft();
    QSize size(rcFinished.width(), rcFinished.height());
    completePoint(cachePoints);
    if (topLeft.toPoint() != sourceProxy->pos()) {
        sourceProxy->setPos(topLeft);
        sourceProxy->setSize(size);
        for (int i = 0; i < cachePoints.size(); i++) {
            cachePoints[i] = cachePoints[i] - topLeft.toPoint();
        }
        proxyGraphics->setPos(topLeft);
    }

    adjustBoundingRect(QRectF(0, 0, size.width(), size.height()));
    proxyGraphics->applyTransformChanged();
    setAnnotationPoints(cachePoints);
    update();
    setStatus(kStatusFinished);
    createInternalGraphics();
    proxyGraphics->setFlag(ItemIsSelectable, true);
}

void GeometricGraphicsAnnotation::initProperty()
{
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    if (sourceProxy->getAnnotationMap().isEmpty()) {
        QVariantMap properties;
        properties[Annotation::segmentColor] = QColor(GKD::WIRE_NORMAL_COLOR);
        properties[Annotation::segmentWidth] = 3;
        properties[Annotation::pointArray] = QPolygon();
        properties[Annotation::segmentStyle] = 1; // qt solid line
        properties[Annotation::isShowHeadArrow] = false;
        if (annotationType == AnnotationSourceGraphicsObject::kBrokenLineArrowAnnotation
            || annotationType == AnnotationSourceGraphicsObject::kLineSegmentArrow) {
            properties[Annotation::isShowArrow] = true;
        } else {
            properties[Annotation::isShowArrow] = false;
        }
        sourceProxy->resetAnnotationMap(properties);
    }
}

void GeometricGraphicsAnnotation::completePoint(QPolygonF &pointArray)
{
    if (annotationType == kCircleAnnotation || annotationType == kRectAnnotation) {
        if (pointArray.size() == 2) {
            QPointF p1 = pointArray[0];
            QPointF p2 = pointArray[1];
            QPointF p3(p2.x(), p1.y());
            QPointF p4(p1.x(), p2.y());
            pointArray.clear();
            pointArray << p1 << p3 << p2 << p4;
        }
    }
}

void GeometricGraphicsAnnotation::drawPointsRect(QPainter *painter)
{
    if (!(status & kStatusFinished) || annotationType == kArcAnnotation) {
        return;
    }
    QBrush brush = painter->brush();
    QPen pen = painter->pen();
    painter->setPen(QPen(1));
    painter->setBrush(QColor(0x2E9FE6));
    for each (auto point in cachePoints) {
        QRectF pointRc(point.x() - controlSize, point.y() - controlSize, 2 * controlSize, 2 * controlSize);
        painter->drawEllipse(pointRc);
    }
    painter->setBrush(brush);
    painter->setPen(pen);
}

void GeometricGraphicsAnnotation::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return GraphicsLayer::mousePressEvent(event);
    }
    QPointF clickPoint = event->scenePos();
    for each (auto point in cachePoints) {
        QPointF sencePoint = mapToScene(point);
        QRectF pointRc(sencePoint.x() - controlSize, sencePoint.y() - controlSize, 2 * controlSize, 2 * controlSize);
        if (pointRc.contains(clickPoint)) { }
    }
    GraphicsLayer::mousePressEvent(event);
}

QPolygonF GeometricGraphicsAnnotation::getCachePoints()
{
    return cachePoints;
}

void GeometricGraphicsAnnotation::createInternalGraphics()
{

    annotationPointGraphics = AnnotationPoint::createAllAnnotationGraphics(getCanvasScene(), cachePoints);
    QListIterator<AnnotationPoint *> AnnotationPointIter(annotationPointGraphics);
    while (AnnotationPointIter.hasNext()) {
        AnnotationPoint *AnnotationPointGraph = AnnotationPointIter.next();
        if (AnnotationPointGraph) {
            AnnotationPointGraph->setParentItem(this);
            connect(AnnotationPointGraph, SIGNAL(positionChanged(QPointF, int, annotationChange)), this,
                    SLOT(onPointChange(QPointF, int, annotationChange)));
            AnnotationPointGraph->setZValue(AnnotationPointGraph->zValue() + 1);
        }
    }
    refreshAnnotationPoint();
}

void GeometricGraphicsAnnotation::refreshAnnotationPoint(bool isSelected)
{
    if (!isInterfactionFinished() || annotationPointGraphics.size() <= 0) {
        return;
    }
    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return;
    }
    if (!canvasContext->canModify()) {
        isSelected = false;
    }
    for each (AnnotationPoint *annotationPoint in annotationPointGraphics) {
        if (annotationPoint) {
            annotationPoint->getLayerProperty().setVisible(isSelected);
            annotationPoint->userShow(isSelected);
        }
    }
    QListIterator<AnnotationPoint *> annotationPointIter(annotationPointGraphics);
    while (annotationPointIter.hasNext()) {
        AnnotationPoint *graphics = annotationPointIter.next();
        if (graphics) {
            graphics->setPos(cachePoints[graphics->pointIndex]);
        }
    }
}

void GeometricGraphicsAnnotation::onPointChange(QPointF delta, int index, annotationChange state)
{
    auto canvasContext = getCanvasContext();
    if (canvasContext) {
        auto interactioMode = canvasContext->getInteractionMode();
        // 区域放大模式下不允许调节
        if (interactioMode == kAreaAmplification) {
            return;
        }
    }

    auto proxyGraphics = getTransformProxyGraphicsObject();
    switch (state) {
    case startedChange:
        startChangePoints = cachePoints;
        break;
    case inChange:
        changeCachePoints(delta, index);
        break;
    case finishedChange:
        changeCachePoints(delta, index);
        if (proxyGraphics) {
            refreshAnnotationPoint(proxyGraphics->isSelected());
        }
        setAnnotationPoints(cachePoints);
    default:
        break;
    }
    update();
}

void GeometricGraphicsAnnotation::changeCachePoints(QPointF delta, int index)
{
    switch (annotationType) {
    case kCircleAnnotation:
    case kRectAnnotation: {
        QPolygonF pointF;
        // 圆形所在矩形由第一个点和第四个点组成
        if (index == 0 || index == 2) {
            cachePoints[index] = startChangePoints[index] + delta;
            pointF << cachePoints[0] << cachePoints[2];
        } else if (index == 1) {
            pointF << QPointF(startChangePoints[0].x(), startChangePoints[0].y() + delta.y());
            pointF << QPointF(startChangePoints[2].x() + delta.x(), startChangePoints[2].y());
        } else if (index == 3) {
            pointF << QPointF(startChangePoints[0].x() + delta.x(), startChangePoints[0].y());
            pointF << QPointF(startChangePoints[2].x(), startChangePoints[2].y() + delta.y());
        }
        completePoint(pointF);
        cachePoints = pointF;
    } break;
    case kLineSegment:
    case kLineSegmentArrow:
    case kBrokenLineAnnotation:
    case kBrokenLineArrowAnnotation:
    case kPolygonAnnotation: {
        cachePoints[index] = startChangePoints[index] + delta;
    } break;
    default:
        break;
    }
}

QRectF GeometricGraphicsAnnotation::arcCalcRect(QPointF centerPos, QPointF endPos)
{
    int maxDistance = qAbs(centerPos.x() - endPos.x());
    maxDistance = maxDistance > qAbs(centerPos.y() - endPos.y()) ? maxDistance : qAbs(centerPos.y() - endPos.y());
    QPointF topLeft, bottomRight;
    topLeft.setX(centerPos.x() - maxDistance);
    bottomRight.setX(centerPos.x() + maxDistance);
    topLeft.setY(centerPos.y() - maxDistance);
    bottomRight.setY(centerPos.y() + maxDistance);
    return QRectF(topLeft, bottomRight);
}

QPainterPath GeometricGraphicsAnnotation::getArcPath(QPointF point1, QPointF point2, QPointF point3)
{

    QRectF rc = arcCalcRect(point1, point2);
    qreal startAngle = Utility::lineAngle(point1.toPoint(), point2.toPoint());
    qreal endAngle = Utility::lineAngle(point1.toPoint(), point3.toPoint());
    // painter->drawArc(rc, -16 * startAngle, (startAngle-  endAngle)*16);
    QPainterPath path;
    path.arcMoveTo(rc, -startAngle);
    path.arcTo(rc, -startAngle, (startAngle - endAngle));
    return path;
}

bool GeometricGraphicsAnnotation::IsVailid()
{
    bool validFlag = false;
    switch (annotationType) {
    case kCircleAnnotation:
    case kRectAnnotation: // 矩形和圆暂时不校验
        validFlag = true;
        break;
    case kLineSegment:
    case kLineSegmentArrow:
    case kBrokenLineAnnotation:
    case kBrokenLineArrowAnnotation:
    case kPolygonAnnotation: {
        if (cachePoints.size() < 2) {
            break;
        }
        for (int i = 1; i < cachePoints.size(); ++i) {
            QLineF line(cachePoints[i], cachePoints[i - 1]);
            // 如果有连续两点间距离大于3 那么就是合法的 否则这次绘制的图形不合法
            if (line.length() > 3) {
                validFlag = true;
            }
        }

    } break;
    case kArcAnnotation:
        if (cachePoints.size() >= 3) {
            validFlag = true;
        }
        break;
    default:
        break;
    }

    return validFlag;
}