#ifndef STEPSOURCEGRAPHICSOBJECT_H
#define STEPSOURCEGRAPHICSOBJECT_H

#include "ControlModuleSourceGraphicsObject.h"
#include "defines.h"

class SelftDrawSvgGraphicsObject;
class StepSourceGraphicsObject : public ControlModuleSourceGraphicsObject
{
    Q_OBJECT

public:
    StepSourceGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                QGraphicsItem *parent = 0);
    virtual ~StepSourceGraphicsObject();

    virtual void createInternalGraphics();

    virtual void selfSvgPaint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    virtual void updatePosition();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    SelftDrawSvgGraphicsObject *m_selfSvg;
};

#endif // STEPSOURCEGRAPHICSOBJECT_H