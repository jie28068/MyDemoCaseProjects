#include "StraightLineConnectorWireAlgorithm.h"
#include "PortGraphicsObject.h"

StraightLineConnectorWireAlgorithm::StraightLineConnectorWireAlgorithm(QObject *parent)
    : IConnectorWireAlgorithm(parent)
{
}

StraightLineConnectorWireAlgorithm::~StraightLineConnectorWireAlgorithm() { }

QPolygonF StraightLineConnectorWireAlgorithm::getAlgorithmPath(QPointF startPos, QPointF startExternalPos,
                                                               QPointF finishPos, QPointF finishExternalPos,
                                                               QPainterPath obstaclePath)
{
    QPolygonF points;
    points.append(startPos);
    points.append(finishPos);
    return points;
}

QString StraightLineConnectorWireAlgorithm::getAlgorithmName()
{
    return StraightLineAlgorithmName;
}