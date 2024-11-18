#include "BezierConnectorWireAlgorithm.h"
#include "PortGraphicsObject.h"

#include <QDebug>

BezierConnectorWireAlgorithm::BezierConnectorWireAlgorithm(QObject *parent) : IConnectorWireAlgorithm(parent) { }

BezierConnectorWireAlgorithm::~BezierConnectorWireAlgorithm() { }

QString BezierConnectorWireAlgorithm::getAlgorithmName()
{
    return BezierAlogrithmName;
}

QPolygonF BezierConnectorWireAlgorithm::getAlgorithmPath(QPointF startPos, QPointF startExternalPos, QPointF finishPos,
                                                         QPointF finishExternalPos, QPainterPath obstaclePath)
{
    QPolygonF points;
    QPointF startPoint = startPos;
    QPointF endPoint = finishPos;
    QPainterPath path;

    path.moveTo(startPoint);
    QPointF c1(endPoint.rx(), startPoint.ry());
    QPointF c2(startPoint.rx(), endPoint.ry());
    path.cubicTo(c1, c2, endPoint);
    points = path.toFillPolygon();
    if (points.size() > 2) {
        points.pop_back();
    }

    return points;
}