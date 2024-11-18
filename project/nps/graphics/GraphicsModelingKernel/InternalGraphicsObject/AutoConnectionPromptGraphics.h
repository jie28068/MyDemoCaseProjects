#pragma once

#include "GraphicsLayer.h"

class PortGraphicsObject;
class ConnectorWirePrompt;

class AutoConnectionPromptGraphics : public GraphicsLayer
{
    Q_OBJECT
public:
    AutoConnectionPromptGraphics(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    virtual ~AutoConnectionPromptGraphics();

    void clean();

    bool addPortPair(PortGraphicsObject *port1, PortGraphicsObject *port2);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

    virtual QRectF boundingRect() const override;

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    QList<ConnectorWirePrompt *> wirePormpts;

    QRectF unitedRect;
};
