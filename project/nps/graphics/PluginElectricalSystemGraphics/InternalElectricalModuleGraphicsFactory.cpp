#include "InternalElectricalModuleGraphicsFactory.h"
#include "ElectricalModulePortGraphicsObject.h"
#include "ElectricalModuleSourceGraphicsObject.h"

InternalElectricalModuleGraphicsFactory::InternalElectricalModuleGraphicsFactory() : ICanvasGraphicsObjectFactory() { }

SourceGraphicsObject *
InternalElectricalModuleGraphicsFactory::createSourceGraphicsObject(ICanvasScene *canvasScene,
                                                                    QSharedPointer<SourceProxy> source)
{
    return new ElectricalModuleSourceGraphicsObject(canvasScene, source);
}

// 创建资源图元的端口图元对象
PortGraphicsObject *InternalElectricalModuleGraphicsFactory::createPortGraphicsObject(
        ICanvasScene *canvasScene, SourceGraphicsObject *sourceGraphics, QSharedPointer<PortContext> context)
{
    return new ElectricalModulePortGraphicsObject(canvasScene, sourceGraphics, context);
}