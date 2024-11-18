#ifndef GAINSOURCEGRAPHICSOBJECT_H
#define GAINSOURCEGRAPHICSOBJECT_H

/*
        控制系统增益模块
*/

#include "ControlModuleSourceGraphicsObject.h"
#include "defines.h"

class GainMouduleSourceGraphicsObject : public ControlModuleSourceGraphicsObject
{
    Q_OBJECT

public:
    GainMouduleSourceGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                    QGraphicsItem *parent = 0);
    virtual ~GainMouduleSourceGraphicsObject();

    virtual void createInternalGraphics();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
};

#endif // GAINSOURCEGRAPHICSOBJECT_H