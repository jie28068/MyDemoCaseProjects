#include "StateMachinePortGraphicsObject.h"

StateMachinePortGraphicsObject::StateMachinePortGraphicsObject(ICanvasScene *canvasScene,
                                                               SourceGraphicsObject *sourceGraphics,
                                                               QSharedPointer<PortContext> context,
                                                               QGraphicsItem *parent)
    : PortGraphicsObject(canvasScene, sourceGraphics, context, parent)
{
}

StateMachinePortGraphicsObject::~StateMachinePortGraphicsObject() { }
