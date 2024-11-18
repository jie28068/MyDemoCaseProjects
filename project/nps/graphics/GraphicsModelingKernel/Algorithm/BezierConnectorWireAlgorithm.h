#ifndef BEZIERCONNECTORWIREALGORITHM_H
#define BEZIERCONNECTORWIREALGORITHM_H

#include "IConnectorWireAlgorithm.h"

#include <QObject>

class BezierConnectorWireAlgorithm : public IConnectorWireAlgorithm
{
    Q_OBJECT

public:
    BezierConnectorWireAlgorithm(QObject *parent = 0);
    ~BezierConnectorWireAlgorithm();

    virtual QString getAlgorithmName();

    virtual QPolygonF getAlgorithmPath(QPointF startPos, QPointF startExternalPos, QPointF finishPos,
                                       QPointF finishExternalPos, QPainterPath obstaclePath = QPainterPath()) override;

private:
};

#endif // BEZIERCONNECTORWIREALGORITHM_H
