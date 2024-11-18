#include "ICanvasGraphicsObjectFactory.h"
#include "ConnectorWireGraphicsObject.h"
#include "TransformProxyGraphicsObject.h"

#include <QString>

ICanvasGraphicsObjectFactory::ICanvasGraphicsObjectFactory(void) { }

ICanvasGraphicsObjectFactory::~ICanvasGraphicsObjectFactory(void) { }

TransformProxyGraphicsObject *
ICanvasGraphicsObjectFactory::createTransformProxyGraphicsObject(ICanvasScene *weakCanvasScene,
                                                                 QSharedPointer<SourceProxy> source)
{
    if (!weakCanvasScene || source.isNull()) {
        return nullptr;
    }
    return new TransformProxyGraphicsObject(weakCanvasScene, source);
}
