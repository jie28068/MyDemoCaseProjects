#ifndef CONTROLSVGGRAPHICSOBJECT_H
#define CONTROLSVGGRAPHICSOBJECT_H

#include "defines.h"

class ControlSVGGraphicsObject : public SvgGraphicsObject
{
    Q_OBJECT

public:
    ControlSVGGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    ~ControlSVGGraphicsObject();

    virtual QRectF boundingRect() const override;

    virtual void updatePosition() override;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    TransformProxyGraphicsObject *proxyGraphicsObject;
};

#endif // CONTROLSVGGRAPHICSOBJECT_H
