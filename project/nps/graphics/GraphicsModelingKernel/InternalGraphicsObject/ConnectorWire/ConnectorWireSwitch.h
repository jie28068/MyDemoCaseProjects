#ifndef CONNECTORWIRESWITCH_H
#define CONNECTORWIRESWITCH_H

#include "GraphicsLayer.h"

class ConnectorWireGraphicsObject;

class ConnectorWireSwitch : public GraphicsLayer
{
    Q_OBJECT

public:
    ConnectorWireSwitch(ICanvasScene *canvasScene, bool status, QGraphicsItem *parent = 0);
    ~ConnectorWireSwitch();

    friend class ConnectorWireGraphicsObject;

    virtual int type() const override;

    void attachConnectorWire(ConnectorWireGraphicsObject *wire);

    void detachConnectorWire();

protected:
    virtual QRectF boundingRect() const;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void switchStatusChanged(bool status);

public slots:
    void onConnectStatusChanged(bool status);

private:
    void doSwitch(bool isSwitchOn);

private:
    bool isOn;

    ConnectorWireGraphicsObject *connectorWireGraphicsObject;
};

#endif // CONNECTORWIRESWITCH_H
