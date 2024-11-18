#ifndef MANHATTACONNECTORWIREALGORITHM_H
#define MANHATTACONNECTORWIREALGORITHM_H

#include "Global.h"
#include "IConnectorWireAlgorithm.h"

#include <QObject>

/// @brief 曼哈顿路径连接线算法
class GRAPHICSMODELINGKERNEL_EXPORT ManhattaConnectorWireAlgorithm : public IConnectorWireAlgorithm
{
    Q_OBJECT

public:
    ManhattaConnectorWireAlgorithm(QObject *parent = 0);
    ~ManhattaConnectorWireAlgorithm();

    virtual bool portMoving(QPointF movingPos, int movingPortType, QPolygonF &points) override;

    virtual QString getAlgorithmName();

    virtual QPolygonF getAlgorithmPath(QPointF startPos, QPointF startExternalPos, QPointF finishPos,
                                       QPointF finishExternalPos, QPainterPath obstaclePath) override;

private:
    // 计算外联点可以采用曼哈顿路径连线时，拐角最少的一个路径
    bool calcManhattanConnection(QPointF startPos, QPointF endPos, QPointF startExteranLPos, QPointF endExternalPos,
                                 QPolygonF &result);

    QPolygonF calculate(QPointF startPos, QPointF endPos, QPointF startExteranLPos, QPointF endExternalPos);

    void filterSamePoints(QPolygonF &points);

    /// @brief 当pos和externalPos坐标相同时，根据参考点referencePos计算获取坐标pos的外延点externalPos的坐标
    /// @param pos
    /// @param externalPos
    /// @param referencePos
    void fetchExternalPos(QPointF &pos, QPointF &externalPos, QPointF &referencePos);

private:
    Qt::AnchorPoint startAnchor;
    Qt::AnchorPoint endAnchor;
    int gridSpace;
    QPainterPath allTransformProxyPainterPath;
};

#endif // MANHATTACONNECTORWIREALGORITHM_H
