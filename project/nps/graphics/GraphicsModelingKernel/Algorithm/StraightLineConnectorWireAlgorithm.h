#ifndef STRAIGHTLINECONNECTORWIREALGORITHM_H
#define STRAIGHTLINECONNECTORWIREALGORITHM_H

#include "IConnectorWireAlgorithm.h"

#include <QObject>

/// @brief 直线连接线算法
class StraightLineConnectorWireAlgorithm : public IConnectorWireAlgorithm
{
    Q_OBJECT

public:
    StraightLineConnectorWireAlgorithm(QObject *parent = 0);
    ~StraightLineConnectorWireAlgorithm();

    virtual QPolygonF getAlgorithmPath(QPointF startPos, QPointF startExternalPos, QPointF finishPos,
                                       QPointF finishExternalPos, QPainterPath obstaclePath = QPainterPath()) override;

    virtual QString getAlgorithmName();

private:
};

#endif // STRAIGHTLINECONNECTORWIREALGORITHM_H
