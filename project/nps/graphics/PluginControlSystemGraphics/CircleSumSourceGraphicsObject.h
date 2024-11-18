#ifndef CIRCLESUMSOURCEGRAPHICSOBJECT_H
#define CIRCLESUMSOURCEGRAPHICSOBJECT_H

/*
        控制系统圆形加法器


*/

#include "ControlModuleSourceGraphicsObject.h"
#include "defines.h"

class CircleSumSourceGraphicsObject : public ControlModuleSourceGraphicsObject
{
    Q_OBJECT

public:
    CircleSumSourceGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                  QGraphicsItem *parent = 0);
    virtual ~CircleSumSourceGraphicsObject();

    virtual void createInternalGraphics();

public slots:
    void onPortReset(int size);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    virtual bool otherPaint(QPainter *painter, const QRectF rect);

private:
};

#endif // CIRCLESUMSOURCEGRAPHICSOBJECT_H
