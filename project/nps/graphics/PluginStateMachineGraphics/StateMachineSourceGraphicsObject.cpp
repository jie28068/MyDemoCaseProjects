#include "StateMachineSourceGraphicsObject.h"

StateMachineSourceGraphicsObject::StateMachineSourceGraphicsObject(ICanvasScene *canvasScene,
                                                                   QSharedPointer<SourceProxy> source,
                                                                   QGraphicsItem *parent)
    : SourceGraphicsObject(canvasScene, source, parent)
{
}

StateMachineSourceGraphicsObject::~StateMachineSourceGraphicsObject() { }
