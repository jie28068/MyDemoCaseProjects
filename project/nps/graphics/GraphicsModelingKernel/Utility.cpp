#include "Utility.h"
#include "CanvasContext.h"
#include "ICanvasScene.h"
#include "SourceGraphicsObject.h"

#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include <QDateTime>
#include <QPainterPath>
#include <QPen>
#include <QUuid>
#include <qmath.h>

int Utility::getTransformAngle(const QTransform &transform)
{
    qreal radians = qAtan2(transform.m12(), transform.m11());
    qreal angle = radians / M_PI * 180;
    if (angle == -90) {
        angle = 270;
    }
    return qRound(angle);
}

Qt::AnchorPoint Utility::getAnchorPoint(Qt::AnchorPoint anchor, int angle)
{
    switch (angle) {
    case 0:
        return anchor;
        break;
    case 90: {
        if (anchor == Qt::AnchorTop) {
            return Qt::AnchorRight;
        }
        if (anchor == Qt::AnchorRight) {
            return Qt::AnchorBottom;
        }
        if (anchor == Qt::AnchorBottom) {
            return Qt::AnchorLeft;
        }
        if (anchor == Qt::AnchorLeft) {
            return Qt::AnchorTop;
        }
    } break;
    case 180: {
        if (anchor == Qt::AnchorTop) {
            return Qt::AnchorBottom;
        }
        if (anchor == Qt::AnchorRight) {
            return Qt::AnchorLeft;
        }
        if (anchor == Qt::AnchorBottom) {
            return Qt::AnchorTop;
        }
        if (anchor == Qt::AnchorLeft) {
            return Qt::AnchorRight;
        }
    } break;
    case -90:
    case 270: {
        if (anchor == Qt::AnchorTop) {
            return Qt::AnchorLeft;
        }
        if (anchor == Qt::AnchorRight) {
            return Qt::AnchorTop;
        }
        if (anchor == Qt::AnchorBottom) {
            return Qt::AnchorRight;
        }
        if (anchor == Qt::AnchorLeft) {
            return Qt::AnchorBottom;
        }
    } break;
    default:
        break;
    }
    return anchor;
}

bool Utility::pointIsInLine(QPoint pos, QPoint startPos, QPoint endPos)
{
    // 计算叉乘
    if ((pos.x() - startPos.x()) * (endPos.y() - startPos.y()) == (endPos.x() - startPos.x()) * (pos.y() - startPos.y())
        && qMin<int>(startPos.x(), endPos.x()) <= pos.x() && pos.x() <= qMax<int>(startPos.x(), endPos.x())
        && qMin<int>(startPos.y(), endPos.y()) <= pos.y() && pos.y() <= qMax<int>(startPos.y(), endPos.y())) {
        return true;
    }
    return false;
}

bool Utility::checkCollision(QPointF startPos, QPointF endPos, QList<QRectF> collisionRects)
{
    QPainterPath pathLine;
    pathLine.moveTo(startPos);
    pathLine.lineTo(endPos);
    QListIterator<QRectF> rcIter(collisionRects);
    while (rcIter.hasNext()) {
        QRectF rc = rcIter.next();
        if (!rc.isNull()) {
            QPainterPath pathBlock;
            pathBlock.addRect(rc);
            if (pathBlock.intersects(pathLine)) {
                return true;
            }
        }
    }
    return false;
}

bool Utility::checkCollision(QPointF startPos, QPointF endPos, const QPainterPath &path)
{
    QPainterPath pathLine;
    pathLine.moveTo(startPos);
    pathLine.lineTo(endPos);

    return path.intersects(pathLine);
}

int Utility::getRightAngleCounts(QPolygonF points)
{
    if (points.size() <= 2) {
        return 0;
    }
    QVector<QPointF> allPoints;
    for (int i = 0; i < points.size(); i++) {
        // 剔除重复的点
        if (allPoints.size() > 0) {
            if (points[i] != allPoints.last()) {
                allPoints.append(points[i]);
            }
        } else {
            allPoints.append(points[i]);
        }
    }

    int rightAngleNum = 0;
    for (int j = 0; j < allPoints.size() - 2; j++) {
        QLineF line1 = QLineF(allPoints[j], allPoints[j + 1]);
        QLineF line2 = QLineF(allPoints[j + 1], allPoints[j + 2]);
        int line1Angle = line1.angle();
        int line2Angle = line2.angle();
        if (line1Angle != line2Angle) {
            rightAngleNum++;
        }
        // if (int(line1.angle() - line2.angle()) % 180 != 0) {
        //     rightAngleNum++;
        // }
    }

    return rightAngleNum;
}

QString Utility::createUUID()
{
    return QUuid::createUuid().toString();
}

QPointF Utility::pointAlignmentToGrid(QPointF pos, int gridSpace)
{
    if (gridSpace <= 0) {
        return pos;
    }
    qreal xpos = qRound(pos.rx() * 1.0 / gridSpace) * gridSpace;
    qreal ypos = qRound(pos.ry() * 1.0 / gridSpace) * gridSpace;
    return QPointF(xpos, ypos);
}

QSizeF Utility::sizeAlignmentToGrid(QSizeF size, int gridSpace)
{
    if (gridSpace <= 0) {
        return size;
    }
    qreal width = qRound(size.width() * 1.0 / gridSpace) * gridSpace;
    qreal height = qRound(size.height() * 1.0 / gridSpace) * gridSpace;
    return QSizeF(width, height);
}

QRectF Utility::rectAlignmentToGrid(QRectF rect, int gridSpace)
{
    if (gridSpace <= 0) {
        return rect;
    }
    QPointF topLeftPos = rect.topLeft();
    QSizeF size = rect.size();
    return QRectF(pointAlignmentToGrid(topLeftPos, gridSpace), sizeAlignmentToGrid(size, gridSpace));
}

qreal Utility::numberAlignmentHalfGrid(qreal value, qreal gridSpace)
{
    value = qRound(value / gridSpace) * gridSpace;
    qreal halfGridSpace = gridSpace / 2.f;
    if (int(value / halfGridSpace) % 2 == 0) {
        value -= halfGridSpace;
    }
    return value;
}

QPointF Utility::calculateAlignmentPos(SourceGraphicsObject *sourceGraphics, QPointF pos)
{
    if (!sourceGraphics) {
        return pos;
    }
    auto canvasContext = sourceGraphics->getCanvasContext();
    if (!canvasContext) {
        return pos;
    }
    QPointF result = pos;

    qreal gridSpace = canvasContext->gridSpace();
    SourceGraphicsObject::AlignmentType xType = sourceGraphics->getXAxisAlignment();
    if (xType == SourceGraphicsObject::kAlignmentGrid) {
        qreal xPos = pos.x();
        xPos = qRound(xPos / gridSpace) * gridSpace;
        result.setX(xPos);
    } else if (xType == SourceGraphicsObject::kAlignmentHalfGrid) {
        qreal xPos = pos.x();
        xPos = numberAlignmentHalfGrid(xPos, gridSpace);
        result.setX(xPos);
    }

    SourceGraphicsObject::AlignmentType yType = sourceGraphics->getYAxisAlignment();
    if (yType == SourceGraphicsObject::kAlignmentGrid) {
        qreal yPos = pos.y();
        yPos = qRound(yPos / gridSpace) * gridSpace;
        result.setY(yPos);
    } else if (yType == SourceGraphicsObject::kAlignmentHalfGrid) {
        qreal yPos = pos.y();
        yPos = numberAlignmentHalfGrid(yPos, gridSpace);
        result.setY(yPos);
    }

    return result;
}

bool Utility::isEqualZero(qreal value)
{
    return (fabs(value) <= 1e-16);
}

void Utility::qt_graphicsItem_shapeFromPath(QPainterPath &newpath, const QPainterPath &path, const QPen &pen)
{

    // We unfortunately need this hack as QPainterPathStroker will set a width of 1.0
    // if we pass a value of 0.0 to QPainterPathStroker::setWidth()
    const qreal penWidthZero = qreal(0.00000001);

    if (path == QPainterPath() || pen == Qt::NoPen) {
        newpath = path;
        return;
    }
    // return &path;
    QPainterPathStroker ps;
    ps.setCapStyle(pen.capStyle());
    if (pen.widthF() <= 0.0)
        ps.setWidth(penWidthZero);
    else
        ps.setWidth(pen.widthF());
    ps.setJoinStyle(pen.joinStyle());
    ps.setMiterLimit(pen.miterLimit());
    QPainterPath p = ps.createStroke(path);
    p.addPath(path);
    newpath = p;
}

void Utility::rotateOnCenter(QGraphicsItem *item, int angle)
{
    QRectF rectSRT = item->transform().mapRect(item->boundingRect());
    QPointF pointSRT = rectSRT.center();

    QTransform transRotation;
    transRotation.rotate(angle);

    QTransform trans = item->transform();
    trans *= transRotation;
    QRectF rectSR = trans.mapRect(item->boundingRect());
    QPointF pointSR = rectSR.center();

    QPointF detal = pointSRT - pointSR;
    QTransform translateTransform = QTransform::fromTranslate(detal.x(), detal.y());

    item->setTransform(translateTransform, true);
    item->setTransform(transRotation, true);
}

QList<int> Utility::getBinaryOneFlag(int value)
{
    QList<int> result;
    if (value <= 0) {
        return result;
    }
    int index = 0;
    while (value != 0) {
        if (value & 1) {
            result.append(index);
        }
        value = value >> 1;
        index++;
    }
    return result;
}

QString Utility::getRandomString(int length)
{
    QString res;
    qsrand(QDateTime::currentMSecsSinceEpoch());

    const char ch[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int size = sizeof(ch);

    int num = 0;
    for (int i = 0; i < length; ++i) {
        num = rand() % (size - 1);
        res.append(ch[num]);
    }

    return res;
}

int Utility::getAngleIndex(int angle)
{
    int index = 0;
    switch (angle) {
    case 0:
        index = 0;
        break;
    case 90:
        index = 1;
        break;
    case 180:
        index = 2;
        break;
    case -90:
    case 270:
        index = 3;
        break;
    }
    return index;
}

QPointF Utility::calcBusRegulatorPos(QPolygonF points, int angle)
{
    QPointF retPos;

    Qt::AnchorPoint anchorPoint = Qt::AnchorLeft;
    QPointF startPoint = points.at(0);
    QPointF endPoint = points.at(points.size() - 1);
    retPos = endPoint;

    switch (angle) {
    case 0:
    case 180:
        if (startPoint.ry() >= endPoint.ry()) {
            anchorPoint = Qt::AnchorBottom;
        } else {
            anchorPoint = Qt::AnchorTop;
        }
        break;
    case 90:
    case -90:
        if (startPoint.rx() >= endPoint.rx()) {
            anchorPoint = Qt::AnchorRight;
        } else {
            anchorPoint = Qt::AnchorLeft;
        }
        break;
    }

    switch (anchorPoint) {
    case Qt::AnchorLeft:
        retPos = QPointF(retPos.rx() - 5, retPos.ry());
        break;
    case Qt::AnchorRight:
        retPos = QPointF(retPos.rx() + 5, retPos.ry());
        break;
    case Qt::AnchorTop:
        retPos = QPointF(retPos.rx(), retPos.ry() - 5);
        break;
    case Qt::AnchorBottom:
        retPos = QPointF(retPos.rx(), retPos.ry() + 5);
        break;
    default:
        break;
    }

    return retPos;
}

void Utility::polygonfAddOffset(QPolygonF &points, QPointF offset)
{
    for (int i = 0; i < points.size(); i++) {
        points[i] = points[i] + offset;
    }
}

const char *Utility::QStringToChar(const QString &str)
{
    std::string tempStr = str.toStdString();
    return tempStr.c_str();
}

bool Utility::isContainByUUID(QList<QSharedPointer<ConnectorWireContext>> wires,
                              QSharedPointer<ConnectorWireContext> wire)
{
    for each (PConnectorWireContext each in wires) {
        if (each->uuid() == wire->uuid())
            return true;
    }

    return false;
}

qreal Utility::lineAngle(QPoint p1, QPoint p2)
{
    qreal radian = qAtan2((p2.y() - p1.y()), (p2.x() - p1.x()));
    return radian * (180 / 3.1415926);
}

qreal Utility::lineDistance(QPointF p1, QPointF p2)
{
    qreal xDistance = qAbs(p1.x() - p2.x());
    qreal yDistance = qAbs(p1.y() - p2.y());
    return qSqrt(qPow(xDistance, 2) + qPow(yDistance, 2));
}

QPointF Utility::adjustItemChangePos(QRectF sceneRect, QRectF changingRect, QPointF alignMovingPos, QPointF pos)
{
    if (!sceneRect.contains(changingRect)) {
        if (changingRect.top() > sceneRect.top() && changingRect.bottom() < sceneRect.bottom()) {
            if (changingRect.left() < sceneRect.left() || changingRect.right() > sceneRect.right()) {
                alignMovingPos.setX(pos.x());
                return alignMovingPos;
            } else {
                return pos;
            }
        } else if (changingRect.left() > sceneRect.left() && changingRect.right() < sceneRect.right()) {
            if (changingRect.top() < sceneRect.top() || changingRect.bottom() > sceneRect.bottom()) {
                alignMovingPos.setY(pos.y());
                return alignMovingPos;
            } else {
                return pos;
            }
        }
        return pos;
    }
    return QPointF();
}

qreal Utility::getPolygonLineLength(QPolygon polygons)
{
    qreal totalLength = 0;
    if (polygons.size() < 2) {
        return 0;
    }
    for (int i = 0; i < polygons.size() - 1; i++) {
        qreal length = lineDistance(polygons[i], polygons[i + 1]);
        totalLength += length;
    }
    return totalLength;
}

QPointF Utility::calcAnglePoint(int angle, QRectF rc)
{
    angle = angle % 360;
    if (angle > 180 && angle <= 360) {
        angle = angle - 360;
    }
    int proxyWidth = rc.width();
    int proxyHeight = rc.height();
    double xPos = rc.left() + (proxyWidth / 2) + ((proxyWidth / 2)) * qCos(angle / 57.3);
    double yPos = rc.top() + (proxyHeight / 2) + ((proxyHeight / 2)) * qSin(angle / 57.3);
    return QPointF(xPos, yPos);
}

Qt::AlignmentFlag Utility::transAlignment(int alignment)
{
    Qt::AlignmentFlag transAlignment = Qt::AlignCenter;
    switch (alignment) {
    case 1:
        transAlignment = Qt::AlignLeft;
        break;
    case 2:
        transAlignment = Qt::AlignCenter;
        break;
    case 3:
        transAlignment = Qt::AlignRight;
        break;
    default:
        break;
    }
    return transAlignment;
}

QPolygonF Utility::getArrorwPoint(QPointF startPoint, QPointF endPoint, double angle, qreal length)
{
    // 求直线的倾斜角
    double lineAngle = atan2(startPoint.y() - endPoint.y(), endPoint.x() - startPoint.x());
    qreal endX = endPoint.x();
    qreal endY = endPoint.y();
    // 求得箭头点 1 的坐标
    qreal x1 = endX - length * cos(angle + lineAngle);
    qreal y1 = endY + length * sin(angle + lineAngle);
    // 求得箭头点 2 的坐标
    qreal x2 = endX - length * cos(lineAngle - angle);
    qreal y2 = endY + length * sin(lineAngle - angle);
    QPolygonF points;
    points << QPointF(x1, y1) << endPoint << QPointF(x2, y2);
    return points;
}

void Utility::filterSamePoints(QPolygonF &points)
{
    QList<QPointF> listPoints = points.toList();
    if (listPoints.size() <= 2) {
        return;
    }
    QPolygonF result;
    for (int i = 0; i < listPoints.size() - 1;) {
        result.append(listPoints[i]);
        if (listPoints[i].toPoint() == listPoints[i + 1].toPoint()) {
            i = i + 2;
        } else {
            i = i + 1;
        }
    }
    result.append(points.last());
    points = result;
}

CanvasContext::Type Utility::getCanvasTypeByModelType(int boardModelType)
{
    CanvasContext::Type canvastype = CanvasContext::kInvalidType;
    switch (boardModelType) {
    case Kcc::BlockDefinition::ElecBoardModel::Type: // 电气系统
        canvastype = CanvasContext::kElectricalType;
        break;
    case Kcc::BlockDefinition::ControlBoardModel::Type: // 控制系统
        canvastype = CanvasContext::kControlSystemType;
        break;
    case Kcc::BlockDefinition::CombineBoardModel::Type: // 构造型画板
        canvastype = CanvasContext::kUserDefinedType;
        break;
    case Kcc::BlockDefinition::ComplexBoardModel::Type: // 复合模型画板
        canvastype = CanvasContext::kUserDefinedFrameType;
        break;
    case Kcc::BlockDefinition::ElecCombineBoardModel::Type: // 电气构造画板
        canvastype = CanvasContext::kElecUserDefinedType;
        break;
    default:
        break;
    }
    return canvastype;
}

int Utility::getModelTypeByCanvasType(CanvasContext::Type canvaType)
{
    int boardtype = Kcc::BlockDefinition::ElecBoardModel::Type;
    switch (canvaType) {
    case CanvasContext::kElectricalType: // 电气系统
        boardtype = Kcc::BlockDefinition::ElecBoardModel::Type;
        break;
    case CanvasContext::kControlSystemType: // 控制系统
        boardtype = Kcc::BlockDefinition::ControlBoardModel::Type;
        break;
    case CanvasContext::kUserDefinedType: // 构造型画板
        boardtype = Kcc::BlockDefinition::CombineBoardModel::Type;
        break;
    case CanvasContext::kUserDefinedFrameType: // 复合模型画板
        boardtype = Kcc::BlockDefinition::ComplexBoardModel::Type;
        break;
    case CanvasContext::kElecUserDefinedType: // 电气构造画板
        boardtype = Kcc::BlockDefinition::ElecCombineBoardModel::Type;
        break;
    default:
        break;
    }
    return boardtype;
}

Utility::WireSegmentDirection Utility::getSegmentDirection(QPoint startPos, QPoint endPos)
{
    if (startPos == endPos) {
        return kInvalid;
    }
    if (startPos.x() == endPos.x()) {
        if (startPos.y() <= endPos.y()) {
            return Utility::kTopToBottom;

        } else {
            return Utility::kBottomToTop;
        }
    } else if (startPos.y() == endPos.y()) {
        if (startPos.x() <= endPos.x()) {
            return Utility::kLeftToRight;
        } else {
            return Utility::kRightToLeft;
        }
    }
    return kInvalid;
}

QPoint Utility::getBranchPoint(QPolygonF points1, QPolygonF points2, bool filter)
{
    // 把连接线相邻的重复坐标去重
    if (filter) {
        Utility::filterSamePoints(points1);
        Utility::filterSamePoints(points2);
    }

    int size1 = points1.size();
    int size2 = points2.size();

    int loopSize = size1 < size2 ? size1 : size2;
    if (loopSize <= 1) {
        return QPoint();
    }

    int startDifferentIndex = loopSize - 2;

    for (int i = 0; i < loopSize - 1; i++) {
        if (points2[i] == points1[i]) {
            continue;
        }
        startDifferentIndex = i;
        break;
    }

    if (startDifferentIndex < 1) {
        return points1[0].toPoint();
    }
    Utility::WireSegmentDirection direction1 = Utility::getSegmentDirection(points1[startDifferentIndex - 1].toPoint(),
                                                                            points1[startDifferentIndex].toPoint());
    Utility::WireSegmentDirection direction2 = Utility::getSegmentDirection(points2[startDifferentIndex - 1].toPoint(),
                                                                            points2[startDifferentIndex].toPoint());
    if (direction1 != direction2) {
        return points1[startDifferentIndex - 1].toPoint();
    } else {
        QPoint pt1 = points1[startDifferentIndex].toPoint();
        QPoint pt2 = points2[startDifferentIndex].toPoint();
        QVector<QPointF> vec1 = points1.mid(startDifferentIndex, -1);
        QVector<QPointF> vec2 = points2.mid(startDifferentIndex, -1);
        switch (direction1) {
        case Utility::kLeftToRight: {
            if (pt1.x() > pt2.x()) {
                vec1.push_front(pt2);
            } else {
                vec2.push_front(pt1);
            }
        } break;
        case Utility::kRightToLeft: {
            if (pt1.x() < pt2.x()) {
                vec1.push_front(pt2);
            } else {
                vec2.push_front(pt1);
            }
        } break;
        case Utility::kTopToBottom: {
            if (pt1.y() > pt2.y()) {
                vec1.push_front(pt2);
            } else {
                vec2.push_front(pt1);
            }
        } break;
        case Utility::kBottomToTop: {
            if (pt1.y() < pt2.y()) {
                vec1.push_front(pt2);
            } else {
                vec2.push_front(pt1);
            }
        } break;
        default: {
            return QPoint();
        }
        }
        return getBranchPoint(vec1, vec2, false);
    }
}
