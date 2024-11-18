#include "ManhattaConnectorWireAlgorithm.h"
#include "ConnectorWireGraphicsObject.h"
#include "ICanvasScene.h"
#include "PortContext.h"
#include "PortGraphicsObject.h"
#include "SourceGraphicsObject.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"

#include <QDebug>
#include <QRectF>

ManhattaConnectorWireAlgorithm::ManhattaConnectorWireAlgorithm(QObject *parent) : IConnectorWireAlgorithm(parent)
{
    gridSpace = 10;
}

ManhattaConnectorWireAlgorithm::~ManhattaConnectorWireAlgorithm() { }

bool ManhattaConnectorWireAlgorithm::portMoving(QPointF movingPos, int movingPortType, QPolygonF &points)
{
    if (points.size() < 4) {
        return false;
    }

    bool isStartMoved = false, isEndMoved = false;
    if (movingPortType == 1) {
        isStartMoved = true;
    } else if (movingPortType == 2) {
        isEndMoved = true;
    }

    // 连线的直角数
    int rightAngleNum = 0;
    for (int j = 0; j < points.size() - 2; j++) {
        QLineF line1 = QLineF(points[j], points[j + 1]);
        QLineF line2 = QLineF(points[j + 1], points[j + 2]);
        qreal angle1 = line1.angle();
        qreal angle2 = line2.angle();
        if (int(line1.angle() - line2.angle()) % 180 != 0) {
            rightAngleNum++;
        }
    }
    // 如果连线的直角数小于2个，则不支持该模式
    if (rightAngleNum < 2) {
        return false;
    }

    // 移动位置边界的参考点，越过该参考点那么就会触发连接线算法重新计算
    // 参考点是连线的第二个直角点，要根据端点确定顺序
    QPointF referencePoint;
    // 参考点的邻接点
    QPointF approximalPoint;
    // 参考点索引
    int referenceIndex = 0;
    // 邻接点索引
    int approximalIndex = 0;
    int counts = 0;
    for (int i = 0; i < points.size() - 2; i++) {
        QLineF line1 = QLineF(points[i], points[i + 1]);
        QLineF line2 = QLineF(points[i + 1], points[i + 2]);
        if (int(line1.angle() - line2.angle()) % 180 != 0) {
            counts++;
            if (isEndMoved) {
                if (counts == rightAngleNum - 1) {
                    referenceIndex = i + 1;
                    break;
                }
            } else {
                if (counts == 2) {
                    referenceIndex = i + 1;
                    break;
                }
            }
        }
    }

    QPolygonF &m_points = points;

    referencePoint = m_points[referenceIndex];
    if (isEndMoved) {
        approximalIndex = referenceIndex + 1;
        approximalPoint = m_points[approximalIndex];
        while (approximalPoint == referencePoint && approximalIndex < m_points.size() - 1) {
            approximalIndex++;
            approximalPoint = m_points[approximalIndex];
        }
    } else {
        approximalIndex = referenceIndex - 1;
        approximalPoint = m_points[approximalIndex];
        while (approximalPoint == referencePoint && approximalIndex > 0) {
            approximalIndex--;
            approximalPoint = m_points[approximalIndex];
        }
    }

    // 移动中的端点坐标
    QPointF movedEndPoint = movingPos;
    // 端点移动之前的坐标
    QPointF lastEndPoint;
    // 移动的端点在以参考点为原点构成的坐标系中，处于第几象限
    int quadrant = -1;
    if (isStartMoved) {
        lastEndPoint = m_points.first();
    }
    if (isEndMoved) {
        lastEndPoint = m_points.last();
    }

    // 确定拖动点的象限
    if (lastEndPoint.rx() > referencePoint.rx()) {
        if (lastEndPoint.ry() > referencePoint.ry()) {
            quadrant = 4;
        } else {
            quadrant = 1;
        }
    } else {
        if (lastEndPoint.ry() > referencePoint.ry()) {
            quadrant = 3;
        } else {
            quadrant = 2;
        }
    }

    if (qRound(referencePoint.x()) == qRound(lastEndPoint.x())) {
        return false;
    }
    if (qRound(referencePoint.ry()) == qRound(lastEndPoint.ry())) {
        return false;
    }

    bool flag = false;
    switch (quadrant) {
    case 1: {
        if (movedEndPoint.rx() > referencePoint.rx() && movedEndPoint.ry() < referencePoint.ry()) {
            flag = true;
        }
    } break;
    case 2: {
        if (movedEndPoint.rx() < referencePoint.rx() && movedEndPoint.ry() < referencePoint.ry()) {
            flag = true;
        }
    } break;
    case 3: {
        if (movedEndPoint.rx() < referencePoint.rx() && movedEndPoint.ry() > referencePoint.ry()) {
            flag = true;
        }
    } break;
    case 4: {
        if (movedEndPoint.rx() > referencePoint.rx() && movedEndPoint.ry() > referencePoint.ry()) {
            flag = true;
        }
    } break;
    }

    if (flag) {
        // 计算参考点和邻接点连线的方向，看是水平还是垂直
        bool isVertical = false;
        if (referencePoint.rx() == approximalPoint.rx()) {
            isVertical = true;
        }
        int index = approximalIndex;
        if (isVertical) {
            m_points[approximalIndex].setY(movedEndPoint.ry());
            if (isEndMoved) {
                while (++index < m_points.size() - 1) {
                    m_points[index].setY(movedEndPoint.ry());
                    // 计算下拖动的水平偏移量
                    qreal dx = movedEndPoint.rx() - lastEndPoint.rx();
                    m_points[index].setX(m_points[index].rx() + dx);
                }
                m_points.last() = movedEndPoint;
            } else {
                while (--index > 0) {
                    m_points[index].setY(movedEndPoint.ry());
                    // 计算下拖动的水平偏移量
                    qreal dx = movedEndPoint.rx() - lastEndPoint.rx();
                    m_points[index].setX(m_points[index].rx() + dx);
                }
                m_points.first() = movedEndPoint;
            }

        } else {
            m_points[approximalIndex].setX(movedEndPoint.rx());
            if (isEndMoved) {
                while (++index < m_points.size() - 1) {
                    m_points[index].setX(movedEndPoint.rx());
                    qreal dy = movedEndPoint.ry() - lastEndPoint.ry();
                    m_points[index].setY(m_points[index].ry() + dy);
                }
                m_points.last() = movedEndPoint;
            } else {
                while (--index > 0) {
                    m_points[index].setX(movedEndPoint.rx());
                    qreal dy = movedEndPoint.ry() - lastEndPoint.ry();
                    m_points[index].setY(m_points[index].ry() + dy);
                }
                m_points.first() = movedEndPoint;
            }
        }
    }

    return flag;
}

QString ManhattaConnectorWireAlgorithm::getAlgorithmName()
{
    return ManhattaAlgorithmName;
}

QPolygonF ManhattaConnectorWireAlgorithm::getAlgorithmPath(QPointF startPos, QPointF startExternalPos,
                                                           QPointF finishPos, QPointF finishExternalPos,
                                                           QPainterPath obstaclePath)
{
    fetchExternalPos(startPos, startExternalPos, finishExternalPos);
    fetchExternalPos(finishPos, finishExternalPos, startExternalPos);

    allTransformProxyPainterPath = obstaclePath;
    startAnchor = getAnchorPoint(startPos.toPoint(), startExternalPos.toPoint());
    endAnchor = getAnchorPoint(finishPos.toPoint(), finishExternalPos.toPoint());
    QPolygonF points = calculate(startPos, finishPos, startExternalPos, finishExternalPos);
    filterSamePoints(points);
    return points;
}

bool ManhattaConnectorWireAlgorithm::calcManhattanConnection(QPointF startPos, QPointF endPos, QPointF startExternalPos,
                                                             QPointF endExternalPos, QPolygonF &result)
{
    // 此种情况需要看两个外联点的连线都不经过起点和终点
    bool res = false;
    // 两个外联点组成的矩形区域
    QPolygonF externalPolygonF;
    externalPolygonF.append(startExternalPos);
    externalPolygonF.append(endExternalPos);
    QRectF externalRect = externalPolygonF.boundingRect();
    // 宽高为0，属于两外联点x或y坐标相等
    if (externalRect.width() == 0) {
        externalRect.setWidth(1);
        // 判断起始点是否在外联点的连线上
        if (Utility::pointIsInLine(startPos.toPoint(), startExternalPos.toPoint(), endExternalPos.toPoint())
            || Utility::pointIsInLine(endPos.toPoint(), startExternalPos.toPoint(), endExternalPos.toPoint())) {
            return false;
        }
    }
    if (externalRect.height() == 0) {
        externalRect.setHeight(1);
        if (Utility::pointIsInLine(startPos.toPoint(), startExternalPos.toPoint(), endExternalPos.toPoint())
            || Utility::pointIsInLine(endPos.toPoint(), startExternalPos.toPoint(), endExternalPos.toPoint())) {
            return false;
        }
    }
    // 两点是连线是水平或垂直的连线
    if (externalRect.width() == 1
        || externalRect.height() == 1
                && !Utility::checkCollision(startExternalPos, endExternalPos, allTransformProxyPainterPath)) {
        result.clear();
        result.append(startPos);
        result.append(startExternalPos);
        result.append(endExternalPos);
        result.append(endPos);
        return true;
    }
    // 曼哈顿路径有两个连线，分别计算,然后取直角数最少的一种方式
    QPointF midPos;
    // 第一种连线
    midPos = QPointF(startExternalPos.rx(), endExternalPos.ry());
    // 连线的碰撞检测
    int num = 100;
    if (!Utility::pointIsInLine(midPos.toPoint(), startPos.toPoint(), startExternalPos.toPoint())
        && !Utility::pointIsInLine(midPos.toPoint(), endPos.toPoint(), endExternalPos.toPoint())
        && !Utility::checkCollision(startExternalPos, midPos, allTransformProxyPainterPath)
        && !Utility::checkCollision(midPos, endExternalPos, allTransformProxyPainterPath)) {
        QPolygonF tempPolygon;
        tempPolygon.append(startPos);
        tempPolygon.append(startExternalPos);
        tempPolygon.append(midPos);
        tempPolygon.append(endExternalPos);
        tempPolygon.append(endPos);
        num = Utility::getRightAngleCounts(tempPolygon);
        result.clear();
        result = tempPolygon;
        res = true;
    }
    // 第二种连线
    midPos = QPointF(endExternalPos.rx(), startExternalPos.ry());
    if (!Utility::pointIsInLine(midPos.toPoint(), startPos.toPoint(), startExternalPos.toPoint())
        && !Utility::pointIsInLine(midPos.toPoint(), endPos.toPoint(), endExternalPos.toPoint())
        && !Utility::checkCollision(startExternalPos, midPos, allTransformProxyPainterPath)
        && !Utility::checkCollision(midPos, endExternalPos, allTransformProxyPainterPath)) {
        QPolygonF tempPolygon;
        tempPolygon.append(startPos);
        tempPolygon.append(startExternalPos);
        tempPolygon.append(midPos);
        tempPolygon.append(endExternalPos);
        tempPolygon.append(endPos);
        int tempNum = Utility::getRightAngleCounts(tempPolygon);
        if (tempNum < num) {
            result.clear();
            result = tempPolygon;
        }
        res = true;
    }
    return res;
}

QPolygonF ManhattaConnectorWireAlgorithm::calculate(QPointF startPos, QPointF endPos, QPointF startExternalPoint,
                                                    QPointF endExternalPoint)
{
    if ((startExternalPoint - endExternalPoint).manhattanLength() <= 2 * gridSpace) {
        // 如果两个连接点之间的曼哈顿距离小于等于2个网格, 那么连接线就用两点的曼哈顿路径
        QPolygonF points;
        points.append(startPos);
        points.append(startExternalPoint);
        QPointF midPos = QPointF(endExternalPoint.rx(), startExternalPoint.ry());
        if (Utility::pointIsInLine(midPos.toPoint(), startPos.toPoint(), startExternalPoint.toPoint())
            || Utility::pointIsInLine(midPos.toPoint(), endPos.toPoint(), endExternalPoint.toPoint())) {
            midPos = QPointF(startExternalPoint.rx(), endExternalPoint.ry());
        }
        points.append(midPos);
        points.append(endExternalPoint);
        points.append(endPos);
        return points;
    }

    QPolygonF result;
    bool flag = calcManhattanConnection(startPos, endPos, startExternalPoint, endExternalPoint, result);
    if (flag) {
        return result;
    }

    // 如果两点的曼哈顿路径不能满足碰撞检测的要求，那就需要动态查找满足碰撞检测的连线拐点了
    // 以端点的外联点计算方向为基准
    if (endAnchor == Qt::AnchorLeft || endAnchor == Qt::AnchorRight) {
        // 查找一条水平线连接外联点的线两端
        //  水平线在y轴移动的步长，区分方向，小于0向上移动，大于0向下移动
        int step = gridSpace;
        if (endPos.ry() < startPos.ry()) {
            step = -gridSpace;
        }

        if (startAnchor == Qt::AnchorTop) {
            step = -gridSpace;
        }
        if (startAnchor == Qt::AnchorBottom) {
            step = gridSpace;
        }

        // x1Pos保存的是起点外联点的延长坐标
        qreal x1Pos = startExternalPoint.rx();
        // x2Pos保存的是终点外联点的延长坐标
        qreal x2Pos = endExternalPoint.rx();
        qreal yPos;
        QPointF pos1, pos2;
        int counts = 0;
        if (true) {
            // 如果水平中线和连接两端的图元发生碰撞，那么需要移动坐标查找
            while (true) {
                yPos = startExternalPoint.ry() + step * counts;
                pos1 = QPointF(startExternalPoint.rx(), yPos);
                pos2 = QPointF(endExternalPoint.rx(), yPos);
                if (!Utility::checkCollision(pos1, pos2, allTransformProxyPainterPath)
                    && !Utility::pointIsInLine(pos1.toPoint(), startPos.toPoint(), startExternalPoint.toPoint())
                    && !Utility::pointIsInLine(pos2.toPoint(), endPos.toPoint(), endExternalPoint.toPoint())) {
                    break;
                }
                counts++;
            }
        }

        // 水平中线的Y坐标确定之后，如果开始端点的方向也是水平方向则需要检查两个外联点和水平线连接的垂直线是否有和图元碰撞

        // 先检查起点的
        if (Utility::checkCollision(startExternalPoint, QPointF(startExternalPoint.rx(), yPos),
                                    allTransformProxyPainterPath)) {
            // 需要移动起点外联点x坐标
            if (startAnchor == Qt::AnchorRight) {
                // 只能正向移动
                step = gridSpace;
            } else {
                step = -gridSpace;
            }
            counts = 1;
            while (true) {
                x1Pos = startExternalPoint.rx() + step * counts;
                pos1 = QPointF(x1Pos, startExternalPoint.ry());
                pos2 = QPointF(x1Pos, yPos);
                if (!Utility::checkCollision(pos1, pos2, allTransformProxyPainterPath)) {
                    break;
                }
                counts++;
            }
        }

        // 检查终点的
        if (Utility::checkCollision(endExternalPoint, QPointF(endExternalPoint.rx(), yPos),
                                    allTransformProxyPainterPath)) {
            // 需要移动起点外联点x坐标
            if (endAnchor == Qt::AnchorRight) {
                // 只能正向移动
                step = gridSpace;
            } else {
                step = -gridSpace;
            }
            counts = 1;
            while (true) {
                x2Pos = endExternalPoint.rx() + step * counts;
                pos1 = QPointF(x2Pos, endExternalPoint.ry());
                pos2 = QPointF(x2Pos, yPos);
                if (!Utility::checkCollision(pos1, pos2, allTransformProxyPainterPath)) {
                    break;
                }
                counts++;
            }
        }

        // 拐点坐标以及计算完成
        result.clear();
        result.append(startPos);
        result.append(startExternalPoint);
        result.append(QPointF(x1Pos, startExternalPoint.ry()));
        result.append(QPointF(x1Pos, yPos));
        result.append(QPointF(x2Pos, yPos));
        result.append(QPointF(x2Pos, endExternalPoint.ry()));
        result.append(endExternalPoint);
        result.append(endPos);
        return result;
    } else {
        // 终点外联线方向为垂直方向
        // 查找一条垂直线连接外联点的线两端
        // 垂直线在x轴移动的步长，区分方向，小于0向上移动，大于0向下移动
        int step = gridSpace;
        if (endPos.rx() < startPos.rx()) {
            step = -gridSpace;
        }
        if (startAnchor == Qt::AnchorLeft) {
            step = -gridSpace;
        }
        if (startAnchor == Qt::AnchorRight) {
            step = gridSpace;
        }
        qreal xPos;
        qreal y1Pos = startExternalPoint.ry();
        qreal y2Pos = endExternalPoint.ry();

        QPointF pos1, pos2;
        int counts = 0;

        while (true) {
            xPos = startExternalPoint.rx() + step * counts;
            pos1 = QPointF(xPos, y1Pos);
            pos2 = QPointF(xPos, y2Pos);
            if (!Utility::checkCollision(pos1, pos2, allTransformProxyPainterPath)
                && !Utility::pointIsInLine(pos1.toPoint(), startPos.toPoint(), startExternalPoint.toPoint())
                && !Utility::pointIsInLine(pos2.toPoint(), endPos.toPoint(), endExternalPoint.toPoint())) {
                break;
            }
            counts++;
        }
        // 垂直中线的x坐标确定之后，那么要检查两端和该垂直中线的两端连线(水平线)是否和图元有碰撞
        // 检查起点外联点和上面查找到的垂直线连线的水平连接线是否和图元有碰撞

        if (Utility::checkCollision(startExternalPoint, QPointF(xPos, startExternalPoint.ry()),
                                    allTransformProxyPainterPath)) {
            if (startAnchor == Qt::AnchorTop) {
                step = -gridSpace;
            } else {
                step = gridSpace;
            }
            counts = 1;
            while (true) {
                y1Pos = startExternalPoint.ry() + step * counts;
                pos1 = QPointF(startExternalPoint.rx(), y1Pos);
                pos2 = QPointF(xPos, y1Pos);
                if (!Utility::checkCollision(pos1, pos2, allTransformProxyPainterPath)) {
                    break;
                }
                counts++;
            }
        }

        // 计算终点
        if (Utility::checkCollision(endExternalPoint, QPointF(xPos, endExternalPoint.ry()),
                                    allTransformProxyPainterPath)) {
            if (endAnchor == Qt::AnchorBottom) {
                step = gridSpace;
            } else {
                step = -gridSpace;
            }
            counts = 1;
            while (true) {
                y2Pos = endExternalPoint.ry() + step * counts;
                pos1 = QPointF(endExternalPoint.rx(), y2Pos);
                pos2 = QPointF(xPos, y2Pos);
                if (!Utility::checkCollision(pos1, pos2, allTransformProxyPainterPath)) {
                    break;
                }
                counts++;
            }
        }
        // 拐点坐标以及计算完成
        result.clear();
        result.append(startPos);
        result.append(startExternalPoint);
        result.append(QPointF(startExternalPoint.rx(), y1Pos));
        result.append(QPointF(xPos, y1Pos));
        result.append(QPointF(xPos, y2Pos));
        result.append(QPointF(endExternalPoint.rx(), y2Pos));
        result.append(endExternalPoint);
        result.append(endPos);
        return result;
    }
}

void ManhattaConnectorWireAlgorithm::filterSamePoints(QPolygonF &points)
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

void ManhattaConnectorWireAlgorithm::fetchExternalPos(QPointF &pos, QPointF &externalPos, QPointF &referencePos)
{
    // 当两个坐标相同时才计算
    if (pos != externalPos) {
        return;
    }
    Qt::AnchorPoint anchor;
    if (abs(pos.ry() - referencePos.ry()) <= 30) {
        if (pos.rx() > referencePos.rx()) {
            anchor = Qt::AnchorLeft;
        } else {
            anchor = Qt::AnchorRight;
        }
    } else {
        if (pos.ry() > referencePos.ry()) {
            anchor = Qt::AnchorTop;
        } else {
            anchor = Qt::AnchorBottom;
        }
    }

    switch (anchor) {
    case Qt::AnchorLeft:
        externalPos = QPointF(pos.rx() - 30, pos.ry());
        break;
    case Qt::AnchorRight:
        externalPos = QPointF(pos.rx() + 30, pos.ry());
        break;
    case Qt::AnchorTop:
        externalPos = QPointF(pos.rx(), pos.ry() - 30);
        break;
    case Qt::AnchorBottom:
        externalPos = QPointF(pos.rx(), pos.ry() + 30);
        break;
    default: {
    }
    }
}
