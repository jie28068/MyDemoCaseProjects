#include "CanvasUndoCommand.h"
#include "CanvasContext.h"

CanvasUndoCommand::CanvasUndoCommand(ICanvasScene *scene, QUndoCommand *parent) : QUndoCommand(parent)
{
    ignoreRedo = false;
    canvasScene = scene;
}

bool CanvasUndoCommand::isEmpty()
{
    return false;
}

void CanvasUndoCommand::setIgnoreRedo(bool ignore)
{
    ignoreRedo = ignore;
}

void CanvasUndoCommand::startVerifyCanvas()
{
    if (!canvasScene || canvasScene->getCanvasContext().isNull()) {
        return;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (canvasContext) {
        BusinessHooksServer *businessHook = canvasContext->getBusinessHooksServer();
        if (!businessHook) {
            return;
        }
        businessHook->verifyCanvasStatus();
    }
}

BusinessHooksServer *CanvasUndoCommand::getBusinessHookServer()
{
    if (!canvasScene) {
        return nullptr;
    }
    auto canvaContext = canvasScene->getCanvasContext();
    if (!canvaContext) {
        return nullptr;
    }
    return canvaContext->getBusinessHooksServer();
}
