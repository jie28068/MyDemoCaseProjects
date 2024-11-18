#ifndef STATEMACHINESOURCEGRAPHICSOBJECT_H
#define STATEMACHINESOURCEGRAPHICSOBJECT_H

#include "defines.h"

class StateMachineSourceGraphicsObject : public SourceGraphicsObject
{
    Q_OBJECT

public:
    StateMachineSourceGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                     QGraphicsItem *parent = 0);
    ~StateMachineSourceGraphicsObject();

private:
};

#endif // STATEMACHINESOURCEGRAPHICSOBJECT_H
