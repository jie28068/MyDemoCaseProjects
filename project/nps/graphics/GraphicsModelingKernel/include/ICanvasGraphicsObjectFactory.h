#pragma once
#include "Global.h"

class SourceProxy;
class PortContext;
class ConnectorWireContext;
class SourceGraphicsObject;
class PortGraphicsObject;
class ConnectorWireGraphicsObject;
class ICanvasScene;
class TransformProxyGraphicsObject;

/// @brief 图元工厂接口类
class GRAPHICSMODELINGKERNEL_EXPORT ICanvasGraphicsObjectFactory
{
public:
    ICanvasGraphicsObjectFactory(void);
    virtual ~ICanvasGraphicsObjectFactory(void);

    /// @brief 创建资源对应的图元对象
    /// @param source 资源实例
    /// @return
    virtual SourceGraphicsObject *createSourceGraphicsObject(ICanvasScene *canvasScene,
                                                             QSharedPointer<SourceProxy> source) = 0;

    /// @brief 创建资源图元上的端口图元对象
    /// @param sourceGraphics 资源图元
    /// @param context 端口信息
    /// @return
    virtual PortGraphicsObject *createPortGraphicsObject(ICanvasScene *canvasScene,
                                                         SourceGraphicsObject *sourceGraphics,
                                                         QSharedPointer<PortContext> context) = 0;

    /// @brief 创建代理图元
    /// @param canvasScene 场景
    /// @param source 资源
    /// @return
    virtual TransformProxyGraphicsObject *createTransformProxyGraphicsObject(ICanvasScene *canvasScene,
                                                                             QSharedPointer<SourceProxy> source);
};