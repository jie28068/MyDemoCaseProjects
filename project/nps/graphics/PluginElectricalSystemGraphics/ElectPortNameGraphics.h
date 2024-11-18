#ifndef ELECPORTNAMEGRAPHICS_H
#define ELECPORTNAMEGRAPHICS_H

#include "defines.h"

/// @brief 端口文字描述图层
class  ElectPortNameGraphics : public PortNameTextGraphicsObject
{
    Q_OBJECT
public:
    ElectPortNameGraphics(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);

    virtual void updatePosition();

    virtual QRectF boundingRect();
};

#endif // ELECPORTNAMEGRAPHICS_H