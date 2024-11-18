#pragma once

#include "ElectricalBaseBusbarObject.h"
#include "ElectricalPhaseLineGraphicsObject.h"

/// @brief 电气母线
class ElectricalBusbarGraphicsObject : public ElectricalBaseBusbarObject
{
    Q_OBJECT

public:
    ElectricalBusbarGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    ~ElectricalBusbarGraphicsObject();

    virtual void createInternalGraphics();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */) override;

    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    virtual QRectF boundingRect() const override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    ElectricalPhaseLineGraphicsObject *phaseLine;
};
