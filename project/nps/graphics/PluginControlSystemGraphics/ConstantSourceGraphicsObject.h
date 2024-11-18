#ifndef CONSTANTSOURCEGRAPHICSOBJECT_H
#define CONSTANTSOURCEGRAPHICSOBJECT_H

/*
        控制系统常数模块


*/

#include "ControlModuleSourceGraphicsObject.h"
#include "defines.h"

class ConstantSourceGraphicsObject : public ControlModuleSourceGraphicsObject
{
    Q_OBJECT

public:
    ConstantSourceGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                 QGraphicsItem *parent = 0);
    virtual ~ConstantSourceGraphicsObject();

    virtual void createInternalGraphics();

private:
};

#endif // CONSTANTSOURCEGRAPHICSOBJECT_H
