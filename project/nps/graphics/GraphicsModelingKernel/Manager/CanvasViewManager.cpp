#include "CanvasViewManager.h"

CanvasViewManager &CanvasViewManager::getInstance()
{
    static CanvasViewManager instance;
    return instance;
}

void CanvasViewManager::addCanvasView(QString id, ICanvasView *view)
{
    if (id.isEmpty() || !view) {
        return;
    }
    canvasViewMap[id] = view;
}

void CanvasViewManager::removeCanvaView(QString id)
{
    canvasViewMap.remove(id);
}

ICanvasView *CanvasViewManager::getCanvasView(QString id)
{
    return canvasViewMap.value(id, nullptr);
}

CanvasViewManager::CanvasViewManager() { }

CanvasViewManager::~CanvasViewManager() { }
