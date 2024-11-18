#ifndef STATEMACHINETRANSFORMPROXYGRAPHICSOBJECT_H
#define STATEMACHINETRANSFORMPROXYGRAPHICSOBJECT_H

#include "defines.h"

class StateMachineTransformProxyGraphicsObject : public TransformProxyGraphicsObject
{
    Q_OBJECT

public:
    StateMachineTransformProxyGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> sourceProxy,
                                             QGraphicsItem *parent = 0);
    ~StateMachineTransformProxyGraphicsObject();

private:
};

#endif // STATEMACHINETRANSFORMPROXYGRAPHICSOBJECT_H
