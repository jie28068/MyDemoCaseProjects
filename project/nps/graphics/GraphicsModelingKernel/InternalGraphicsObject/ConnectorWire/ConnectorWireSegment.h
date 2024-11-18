#ifndef CONNECTWIRESEGMENT_H
#define CONNECTWIRESEGMENT_H

#include "ConnectorWireContext.h"
#include "GraphicsLayer.h"
#include "PortContext.h"

#include <QSet>

class ConnectorWireContext;
class ConnectorWireGraphicsObject;

/// @brief manhatta算法连接线使用的线段类
class ConnectorWireSegment : public GraphicsLayer
{
    Q_OBJECT

public:
    ConnectorWireSegment(ICanvasScene *canvasScene, ConnectorWireGraphicsObject *connectorWire);
    ~ConnectorWireSegment();

    /// @brief 线段方向
    enum SegmentDirection { kNone, kLeftToRight, kRightToLeft, kTopToBottom, kBottomToTop };

    enum SegmentType { kSegmentCommon = 1 << 0, kSegmentFirst = 1 << 1, kSegmentEnd = 1 << 2 };

    SegmentType getSegmentType();

    void setSegmentType(SegmentType type);

    void setPoints(const QPoint &startpt, const QPoint &endpt);

    bool isVertical();

    QPoint getStartPoint();

    QPoint getEndPoint();

    ConnectorWireSegment::SegmentDirection getSegmentDirection();

    /// @brief 检测是否和其他线段有碰撞，不包含属于同一条连接线的线段
    /// @return
    bool isOverLapWithOtherSegment();

    int getDistance();

public:
    virtual int type() const override;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    virtual QPainterPath shape() const;

    virtual QRectF boundingRect() const;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

signals:
    void positionChanged(QPointF nowPos);

    void selectedChanged(bool selected);

private:
    void resetDirection();

    bool canMove();

private:
    /// @brief 开始坐标点
    QPoint startPos;

    /// @brief 结束坐标点
    QPoint endPos;

    ConnectorWireGraphicsObject *attachWire;

    SegmentDirection direction;

    QPoint pressedPos;

    SegmentType segmentType;

    PPortContext oldPortContext;

    PConnectorWireContext oldWireContext;

    QList<PConnectorWireContext> oldWireContextList;
};

#endif // CONNECTWIRESEGMENT_H
