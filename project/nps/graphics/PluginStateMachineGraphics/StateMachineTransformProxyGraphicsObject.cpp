#include "StateMachineTransformProxyGraphicsObject.h"

StateMachineTransformProxyGraphicsObject::StateMachineTransformProxyGraphicsObject(
        ICanvasScene *canvasScene, QSharedPointer<SourceProxy> sourceProxy, QGraphicsItem *parent)
    : TransformProxyGraphicsObject(canvasScene, sourceProxy, parent)
{
}

StateMachineTransformProxyGraphicsObject::~StateMachineTransformProxyGraphicsObject() { }
