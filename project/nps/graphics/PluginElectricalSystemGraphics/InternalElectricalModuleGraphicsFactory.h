#pragma once
#include "defines.h"

class InternalElectricalModuleGraphicsFactory : public ICanvasGraphicsObjectFactory
{
public:
    InternalElectricalModuleGraphicsFactory();

    // 创建资源对应的图元对象
    virtual SourceGraphicsObject *createSourceGraphicsObject(ICanvasScene *canvasScene,
                                                             QSharedPointer<SourceProxy> source);

    // 创建资源图元的端口图元对象
    virtual PortGraphicsObject *createPortGraphicsObject(ICanvasScene *canvasScene,
                                                         SourceGraphicsObject *sourceGraphics,
                                                         QSharedPointer<PortContext> context);
};
