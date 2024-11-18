#pragma once

#include "defines.h"

/// @brief 端口文字描述图层
class IrRegularPortNameTextGraphicsObject : public TextGraphicsObject<QGraphicsTextItem>
{
    Q_OBJECT
public:
    IrRegularPortNameTextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);

    virtual void updatePosition();
};
