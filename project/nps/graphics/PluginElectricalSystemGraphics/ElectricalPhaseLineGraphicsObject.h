#ifndef ELECTRICALPHASELINEGRAPHICSOBJECT_H
#define ELECTRICALPHASELINEGRAPHICSOBJECT_H

#include "defines.h"

/// 电气母线相数线图元
class ElectricalPhaseLineGraphicsObject : public GraphicsLayer
{
    Q_OBJECT

public:
    ElectricalPhaseLineGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    ~ElectricalPhaseLineGraphicsObject();

    virtual void updatePosition() override;

    virtual void userShow(bool visible);

protected:
    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

public slots:
    void onSourceChange(QString key, QVariant val);

private:
    unsigned int gridSpace;

    int currentAngle;

    unsigned int phaseNumber;
};

#endif // ELECTRICALPHASELINEGRAPHICSOBJECT_H
