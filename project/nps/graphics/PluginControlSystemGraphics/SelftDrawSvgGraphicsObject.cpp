#include "SelftDrawSvgGraphicsObject.h"
#include "ControlModuleSourceGraphicsObject.h"

SelftDrawSvgGraphicsObject::SelftDrawSvgGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,ControlModuleSourceGraphicsObject* parentGrahicsObject,
                                                       QGraphicsItem *parent) : GraphicsLayer(canvasScene, parent), parentGraphics(parentGrahicsObject),bourningRect(QRectF())
{
    setLayerFlag(GraphicsLayer::kForegroundLayer);
}

SelftDrawSvgGraphicsObject::~SelftDrawSvgGraphicsObject() { }

QRectF SelftDrawSvgGraphicsObject::boundingRect() const
{
    return bourningRect;
}

void SelftDrawSvgGraphicsObject::setBoundingRect(QRectF rect) 
{ 
    bourningRect = rect;
}

void SelftDrawSvgGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) 
{ 
    if(parentGraphics){
        return parentGraphics->selfSvgPaint(painter,option,widget);
    }
}
