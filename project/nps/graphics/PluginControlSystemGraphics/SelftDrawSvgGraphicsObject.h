#ifndef SELFDRAWSVGGRAPHICSOBJECT_H
#define SELFDRAWSVGGRAPHICSOBJECT_H


#include "defines.h"

class ControlModuleSourceGraphicsObject;
class SelftDrawSvgGraphicsObject : public GraphicsLayer
{
    Q_OBJECT

public:
    SelftDrawSvgGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,ControlModuleSourceGraphicsObject* parentGrahicsObject,
                                 QGraphicsItem *parent = 0);
    virtual ~SelftDrawSvgGraphicsObject();

    virtual QRectF boundingRect() const override;

    void setBoundingRect(QRectF rect);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    ControlModuleSourceGraphicsObject* parentGraphics;
    QRectF bourningRect;
};

#endif // SELFDRAWSVGGRAPHICSOBJECT_H