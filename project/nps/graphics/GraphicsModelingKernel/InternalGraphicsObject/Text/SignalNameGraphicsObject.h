#pragma once

#include "EditableGraphicsTextItem.h"
#include "Global.h"
#include "GraphicsLayer.h"
#include "TextGraphicsObject.h"

class SignalNameGraphicsObject : public TextGraphicsObject<EditableGraphicsTextItem>
{
    Q_OBJECT
public:
    SignalNameGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    virtual ~SignalNameGraphicsObject();

public:
    virtual QRectF boundingRect() const override;

    virtual void updatePosition();

public slots:
    void onTextChanged();

    void onCanvasStatusChanged(CanvasContext::StatusFlag flag);
};
