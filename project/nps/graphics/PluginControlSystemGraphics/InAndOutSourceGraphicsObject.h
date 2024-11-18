#ifndef INANDOUTSOURCEGRAPHICSOBJECT_H
#define INANDOUTSOURCEGRAPHICSOBJECT_H

/*
        自定义构造系统输入输出模块
*/

#include "ControlModuleSourceGraphicsObject.h"
#include "defines.h"

class InAndOutSourceGraphicsObject : public ControlModuleSourceGraphicsObject
{
    Q_OBJECT

public:
    InAndOutSourceGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                 QGraphicsItem *parent = 0);
    virtual ~InAndOutSourceGraphicsObject();

    virtual void createInternalGraphics();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    virtual bool otherPaint(QPainter *painter, const QRectF rect);

private:
};

#endif // INANDOUTSOURCEGRAPHICSOBJECT_H