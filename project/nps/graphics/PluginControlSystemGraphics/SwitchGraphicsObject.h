#ifndef SWITCHGRAPHICSOBJECT_H
#define SWITCHGRAPHICSOBJECT_H

#include "ControlModuleSourceGraphicsObject.h"
#include "defines.h"

class SelftDrawSvgGraphicsObject;
class SwitchGraphicsObject : public ControlModuleSourceGraphicsObject
{
    Q_OBJECT

public:
    SwitchGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                QGraphicsItem *parent = 0);
    virtual ~SwitchGraphicsObject();

    virtual void createInternalGraphics();

    virtual void selfSvgPaint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    SelftDrawSvgGraphicsObject *m_selfSvg;

};

#endif // SWITCHGRAPHICSOBJECT_H