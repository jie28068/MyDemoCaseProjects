#ifndef GRAPHICS_CORE_CONNECTORWIRE_REGULATION_CONTROLLER_H
#define GRAPHICS_CORE_CONNECTORWIRE_REGULATION_CONTROLLER_H

#include "GraphicsLayer.h"
#include <QGraphicsObject>

class ConnectorWireGraphicsObject;
class PortGraphicsObject;

class ConnectorWireRegulationController : public QGraphicsObject
{
    Q_OBJECT
public:
    ConnectorWireRegulationController(ConnectorWireGraphicsObject *wire, QString portType);
    ~ConnectorWireRegulationController();

    friend class ConnectorWireGraphicsObject;
    friend class PortGraphicsObject;

    virtual int type() const override;

    ConnectorWireRegulationController *getMatchedRegulationController();

    ConnectorWireGraphicsObject *getConnectorWire();

    void setHoverFlag(bool value);

    virtual QRectF boundingRect() const;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void positionChanged(const QPointF &pt);

private:
    void processPosChanged(const QPointF &pt);

    void matchAnotherRegulationController(ConnectorWireRegulationController *controller);

private:
    ConnectorWireGraphicsObject *connectorWire;
    bool isHover;
    QString portType;

    ConnectorWireRegulationController *matchedController;
    PortGraphicsObject *matchedPort;

    float regionSize;
};

#endif