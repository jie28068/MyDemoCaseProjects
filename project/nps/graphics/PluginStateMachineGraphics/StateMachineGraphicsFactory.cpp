#include "StateMachineGraphicsFactory.h"
#include "StateMachinePortGraphicsObject.h"
#include "StateMachineSourceGraphicsObject.h"
#include "StateMachineTransformProxyGraphicsObject.h"

StateMachineGraphicsFactory::StateMachineGraphicsFactory() : ICanvasGraphicsObjectFactory() { }

SourceGraphicsObject *StateMachineGraphicsFactory::createSourceGraphicsObject(ICanvasScene *canvasScene,
                                                                              QSharedPointer<SourceProxy> source)
{
    return new StateMachineSourceGraphicsObject(canvasScene, source);
}

// 创建资源图元的端口图元对象
PortGraphicsObject *StateMachineGraphicsFactory::createPortGraphicsObject(ICanvasScene *canvasScene,
                                                                          SourceGraphicsObject *sourceGraphics,
                                                                          QSharedPointer<PortContext> context)
{
    return new StateMachinePortGraphicsObject(canvasScene, sourceGraphics, context);
}

TransformProxyGraphicsObject *
StateMachineGraphicsFactory::createTransformProxyGraphicsObject(ICanvasScene *canvasScene,
                                                                QSharedPointer<SourceProxy> source)
{
    return new StateMachineTransformProxyGraphicsObject(canvasScene, source);
}
