#pragma once

#include "GraphicsLayer.h"

class PortGraphicsObject;

class ConnectorWirePrompt : public QGraphicsObject
{
    Q_OBJECT
public:
    ConnectorWirePrompt(PortGraphicsObject *port1 = nullptr, PortGraphicsObject *port2 = nullptr,
                        QGraphicsItem *parent = 0);
    virtual ~ConnectorWirePrompt();

    virtual QRectF boundingRect() const override;

    QList<QWeakPointer<PortGraphicsObject>> getPortList();

    /// @brief 设置首尾的端点
    void setPorts(PortGraphicsObject *port1, PortGraphicsObject *port2);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

    virtual QPainterPath shape() const;

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:
    void clicked(PortGraphicsObject *port1, PortGraphicsObject *port2);

private:
    QWeakPointer<PortGraphicsObject> weakPort1;
    QWeakPointer<PortGraphicsObject> weakPort2;
    bool isHover;
    QPointF outputPos;
    QPointF inputPos;
    Qt::AnchorPoint inputAnchor;

    /// @brief 两个端点生成的提示连接线的点集合
    QPolygonF points;
};