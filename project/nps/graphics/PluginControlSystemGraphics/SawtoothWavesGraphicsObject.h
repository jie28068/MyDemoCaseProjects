#ifndef SAWTOOTHWAVESGRAPHICSOBJECT_H
#define SAWTOOTHWAVESGRAPHICSOBJECT_H

#include "ControlModuleSourceGraphicsObject.h"
#include "defines.h"

class SelftDrawSvgGraphicsObject;
class SawtoothWavesGraphicsObject : public ControlModuleSourceGraphicsObject
{
    Q_OBJECT

public:
    SawtoothWavesGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                QGraphicsItem *parent = 0);
    virtual ~SawtoothWavesGraphicsObject();

    virtual void createInternalGraphics();

    virtual void selfSvgPaint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    //virtual void updatePosition();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    QPolygonF getPointsByDegress(double degress);

private:
    SelftDrawSvgGraphicsObject *m_selfSvg;
};

#endif // SAWTOOTHWAVESGRAPHICSOBJECT_H