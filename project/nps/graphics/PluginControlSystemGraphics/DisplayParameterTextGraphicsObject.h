#ifndef SHOWPARAMETERTEXTGRAPHICSOBJECT_H
#define SHOWPARAMETERTEXTGRAPHICSOBJECT_H

#include "defines.h"

class DisplayParameterTextGraphicsObject : public TextGraphicsObject<QGraphicsTextItem>
{
    Q_OBJECT

public:
    DisplayParameterTextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    ~DisplayParameterTextGraphicsObject();

    virtual void updatePosition();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /* public slots:
        void onSourceChange(QString, QVariant); */
};

#endif // SHOWPARAMETERTEXTGRAPHICSOBJECT_H