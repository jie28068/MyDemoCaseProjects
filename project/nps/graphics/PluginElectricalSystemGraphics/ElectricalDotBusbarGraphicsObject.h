#pragma once

#include "ElectricalBaseBusbarObject.h"

class ElectricalDotBusbarGraphicsObject : public ElectricalBaseBusbarObject
{
    Q_OBJECT

public:
    ElectricalDotBusbarGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    ~ElectricalDotBusbarGraphicsObject();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */) override;

    virtual QRectF boundingRect() const;
};
