#pragma once

/*
画板视图管理
模型升级时，需要通过canvascontext直接添加同步数据
通过CanvasViewManager提供一个从canvascontext获取图形视图实例的接口
*/

#include "ICanvasView.h"
#include <QMap>

class CanvasViewManager
{
public:
    static CanvasViewManager &getInstance();

    void addCanvasView(QString id, ICanvasView *view);

    void removeCanvaView(QString id);

    ICanvasView *getCanvasView(QString id);

private:
    CanvasViewManager();
    ~CanvasViewManager();

    QMap<QString, ICanvasView *> canvasViewMap;
};
