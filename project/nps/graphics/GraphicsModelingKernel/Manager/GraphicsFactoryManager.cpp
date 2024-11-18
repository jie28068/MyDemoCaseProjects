#include "GraphicsFactoryManager.h"
#include "GraphicsKernelDefinition.h"
#include "InternalAnnotationFactory.h"

GraphicsFactoryManager::GraphicsFactoryManager()
{
    canvasGraphicsObjectFactoryMap[GKD::SOURCE_MODULETYPE_ANNOTATION] =
            QSharedPointer<ICanvasGraphicsObjectFactory>(new InternalAnnotationFactory());
}

GraphicsFactoryManager &GraphicsFactoryManager::getInstance()
{
    static GraphicsFactoryManager instance;
    return instance;
}

bool GraphicsFactoryManager::registerFactory(QString type, QSharedPointer<ICanvasGraphicsObjectFactory> factory)
{
    if (type.isEmpty() || nullptr == factory) {
        return false;
    }
    if (canvasGraphicsObjectFactoryMap.contains(type)) {
        return false;
    }
    canvasGraphicsObjectFactoryMap[type] = factory;
    return true;
}

QSharedPointer<ICanvasGraphicsObjectFactory> GraphicsFactoryManager::getFactory(QString type)
{
    if (type.isEmpty()) {
        return QSharedPointer<ICanvasGraphicsObjectFactory>();
    }
    return canvasGraphicsObjectFactoryMap.value(type, QSharedPointer<ICanvasGraphicsObjectFactory>());
}