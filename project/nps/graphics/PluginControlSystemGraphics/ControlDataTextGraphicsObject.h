#ifndef CONTROLDATATEXTGRAPHICSOBJECT_H
#define CONTROLDATATEXTGRAPHICSOBJECT_H

#include "defines.h"

class ControlDataTextGraphicsObject : public TextGraphicsObject<QGraphicsTextItem>
{
    Q_OBJECT

public:
    ControlDataTextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    ~ControlDataTextGraphicsObject();

    virtual void updatePosition();

    virtual QRectF boundingRect() const;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    // virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    // virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    // virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    // virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};
#endif // CONTROLDATATEXTGRAPHICSOBJECT_H