#pragma once

#include "GraphicsLayer.h"

#include <QSet>

/// @brief 最大图层索引值，因为是根据枚举值计算的索引，而32位程序枚举值计算二进制1的索引位置最多为31
class ICanvasScene;
class CanvasSceneDefaultImpl;
class GraphicsLayerManagerPrivate;
class CanvasContext;
/// @brief 图层图元对象管理类
/// 内部保存的是图元的弱引用
class GRAPHICSMODELINGKERNEL_EXPORT GraphicsLayerManager
{
public:
    // GraphicsLayerManager();
    GraphicsLayerManager(QSharedPointer<CanvasContext> canvasCtx);
    ~GraphicsLayerManager();

    friend class GraphicsLayer;

    /// @brief 获取特定图层的图元列表
    /// @param flag 图层属性
    /// @return
    QList<GraphicsLayer *> getLayers(int flags);

    // 获取所有的图层对象
    QList<GraphicsLayer *> getAllLayers();

private:
    /// @brief 添加图层图元到特定数组索引，数组索引根据图层属性计算得出
    /// @param indexs 数组索引列表
    /// @param layer 图元
    void addLayer(QList<int> indexs, GraphicsLayer *layer);

    /// @brief 移除数组中特定索引的图元
    /// @param indexs 数组索引列表
    /// @param layer 图元
    void removeLayer(QList<int> indexs, GraphicsLayer *layer);

private:
    QScopedPointer<GraphicsLayerManagerPrivate> dataPtr;
};
