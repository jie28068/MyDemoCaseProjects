#ifndef SOURCEPROTOTYPETEXTGRAPHICSOBJECT_H
#define SOURCEPROTOTYPETEXTGRAPHICSOBJECT_H

#include "EditableGraphicsTextItem.h"
#include "Global.h"
#include "TextGraphicsObject.h"

class GRAPHICSMODELINGKERNEL_EXPORT SourceProtoTypeTextGraphicsObject : public TextGraphicsObject<QGraphicsTextItem>
{
    Q_OBJECT

public:
    SourceProtoTypeTextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    ~SourceProtoTypeTextGraphicsObject();

    virtual void updatePosition();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual QRectF boundingRect() const;

    virtual void userShow(bool visible);

private:
    bool isSubSystemPrototype;
};

#endif // SOURCEPROTOTYPETEXTGRAPHICSOBJECT_H
