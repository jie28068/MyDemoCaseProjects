#pragma once

#include "defines.h"

class ElectricalTransformProxyGraphicsObject : public TransformProxyGraphicsObject
{
    Q_OBJECT
public:
    ElectricalTransformProxyGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> sourceProxy,
                                           QGraphicsItem *parent = 0);

    virtual QRectF boundingRect() const override;

private:
    QSharedPointer<SourceProxy> sourceProxy;
};