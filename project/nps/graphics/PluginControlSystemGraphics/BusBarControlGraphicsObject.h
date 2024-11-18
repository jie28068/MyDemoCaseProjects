#ifndef BUSBARCONTROLGRAPHICSOBJECT_H
#define BUSBARCONTROLGRAPHICSOBJECT_H

#include "ControlModuleSourceGraphicsObject.h"
#include "defines.h"

class BusSelectorImage : public GraphicsLayer
{
    Q_OBJECT
public:
    BusSelectorImage(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    virtual QRectF boundingRect() const;

private:
    TransformProxyGraphicsObject *proxyGraphics;
};

class BusBarControlGraphicsObject : public ControlModuleSourceGraphicsObject
{
    Q_OBJECT

public:
    BusBarControlGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                QGraphicsItem *parent = 0);
    virtual ~BusBarControlGraphicsObject();

    virtual void createInternalGraphics();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    BusSelectorImage *imageItem;
};

#endif // BUSBARCONTROLGRAPHICSOBJECT_H