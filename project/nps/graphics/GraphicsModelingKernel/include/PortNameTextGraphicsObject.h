#ifndef PORTNAMETEXTGRAPHICSOBJECT_H
#define PORTNAMETEXTGRAPHICSOBJECT_H

#include "TextGraphicsObject.h"

/// @brief 端口文字描述图层
class GRAPHICSMODELINGKERNEL_EXPORT PortNameTextGraphicsObject : public TextGraphicsObject<QGraphicsTextItem>
{
    Q_OBJECT
public:
    PortNameTextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);

    virtual void updatePosition();

    virtual void userShow(bool visible);
};

#endif // PORTNAMETEXTGRAPHICSOBJECT_H