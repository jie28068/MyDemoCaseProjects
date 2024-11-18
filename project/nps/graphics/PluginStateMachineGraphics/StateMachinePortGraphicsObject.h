#ifndef STATEMACHINEPORTGRAPHICSOBJECT_H
#define STATEMACHINEPORTGRAPHICSOBJECT_H

#include "defines.h"

class StateMachinePortGraphicsObject : public PortGraphicsObject
{
    Q_OBJECT

public:
    StateMachinePortGraphicsObject(ICanvasScene *canvasScene, SourceGraphicsObject *sourceGraphics,
                                   QSharedPointer<PortContext> context, QGraphicsItem *parent = 0);
    ~StateMachinePortGraphicsObject();

private:
};

#endif // STATEMACHINEPORTGRAPHICSOBJECT_H
