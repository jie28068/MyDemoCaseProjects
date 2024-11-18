#ifndef MUTIPORTSWITCHGRAPHICSOBJECT_H
#define MUTIPORTSWITCHGRAPHICSOBJECT_H

#include "ControlModuleSourceGraphicsObject.h"
#include "defines.h"

class SelftDrawSvgGraphicsObject;
class MutiportSwitchGraphicsObject : public ControlModuleSourceGraphicsObject
{
    Q_OBJECT

public:
    MutiportSwitchGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                QGraphicsItem *parent = 0);
    virtual ~MutiportSwitchGraphicsObject();

    virtual void createInternalGraphics();

    virtual void selfSvgPaint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    SelftDrawSvgGraphicsObject *m_selfSvg;

};

#endif // MUTIPORTSWITCHGRAPHICSOBJECT_H