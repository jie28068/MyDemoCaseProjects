#pragma once

#include "BusinessHooksServer.h"
#include "CanvasSceneDefaultImpl.h"
#include "ICanvasScene.h"
#include "TransformProxyGraphicsObject.h"
#include <QUndoCommand>

class CanvasUndoCommand : public QUndoCommand
{
public:
    CanvasUndoCommand(ICanvasScene *scene, QUndoCommand *parent = 0);

    virtual bool isEmpty();

    void setIgnoreRedo(bool ignore);

    /// @brief 校验画板状态
    void startVerifyCanvas();

    BusinessHooksServer *getBusinessHookServer();

protected:
    /// @brief 忽略redo操作
    bool ignoreRedo;
    ICanvasScene *canvasScene;
};
