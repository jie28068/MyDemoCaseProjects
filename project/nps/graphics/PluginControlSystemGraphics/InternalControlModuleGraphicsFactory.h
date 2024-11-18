#pragma once
#include "defines.h"

class InternalControlModuleGraphicsFactory : public ICanvasGraphicsObjectFactory
{
public:
    InternalControlModuleGraphicsFactory();

    /// @brief 创建资源对应的图元对象
    /// @param source 资源实例
    /// @return
    virtual SourceGraphicsObject *createSourceGraphicsObject(ICanvasScene *canvasScene,
                                                             QSharedPointer<SourceProxy> source);

    /// @brief 创建资源图元上的端口图元对象
    /// @param sourceGraphics 资源图元
    /// @param context 端口信息
    /// @return
    virtual PortGraphicsObject *createPortGraphicsObject(ICanvasScene *canvasScene,
                                                         SourceGraphicsObject *sourceGraphics,
                                                         QSharedPointer<PortContext> context);
};