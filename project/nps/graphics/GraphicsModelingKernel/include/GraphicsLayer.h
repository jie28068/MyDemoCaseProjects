#ifndef GRAPHICSLAYER_H
#define GRAPHICSLAYER_H

#include "Global.h"
#include "NPSPropertyManager.h"

#include <QGraphicsObject>
#include <QList>
#include <QScopedPointer>
#include <QString>

class ICanvasScene;
class GraphicsLayerPrivate;
class TransformProxyGraphicsObject;
class TransformItemGroup;
class BusinessHooksServer;
class SourceProxy;
class CanvasContext;

/// @brief 图层基类
class GRAPHICSMODELINGKERNEL_EXPORT GraphicsLayer : public QGraphicsObject
{
    Q_OBJECT

public:
    enum GraphicsLayerFlag {
        kInvalidGraphicsLayer = 0,
        kTransformProxyGraphicsLayer = 1 << 0,        // 代理图层
        kSourceGraphicsLayer = 1 << 1,                // 资源实体
        kPortGraphicsLayer = 1 << 2,                  // 端口
        kConnectorWireGraphicsLayer = 1 << 3,         // 连接线
        kTextGraphicsLayer = 1 << 4,                  // 文本图层
        kCommentGraphicsLayer = 1 << 5,               // 注释图层
        kSelectedGraphicsLayer = 1 << 6,              // 可选择图层
        kMoveableGraphicsLayer = 1 << 7,              // 可移动图层
        kSourceNameGraphicsLayer = 1 << 8,            // 资源名称
        kLegendGraphicsLayer = 1 << 9,                // 图例
        kBusbarPhaseGraphicsLayer = 1 << 10,          // 母线像素标识
        kSizeControlGraphicsLayer = 1 << 11,          // 大小调节控制块
        kRegulator = 1 << 12,                         // 调节器
        kCommentPoint = 1 << 13,                      // 注释模块端口
        kAutoConnectionPromptGraphicsLayer = 1 << 14, // 连接线自动连接提示图层
        kVirtualPortLayer = 1 << 15,                  // 虚拟端口图层
        kBackgroundLayer = 1 << 16,                   // 背景图层
        kForegroundLayer = 1 << 17,                   // 前景图层
        kVirtutalportLayer = 1 << 18,                 // 虚拟端口图层
    };

    GraphicsLayer(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    virtual ~GraphicsLayer();

public:
    /// @brief 把当前图层添加到图元组group中
    /// @param group 图元组对象
    void addToGroup(TransformItemGroup *group);

    /// @brief 把当前图层从图元组group中移除
    /// @param group 图元组对象
    void removeFromGroup(TransformItemGroup *group);

    /// @brief 获取图层属性
    /// @return
    GraphicsLayerFlag getLayerFlag();

    /// @brief 设置图层属性
    /// @param flag
    void setLayerFlag(GraphicsLayerFlag flag);

    /// @brief 获取图元所在的场景
    /// @return
    ICanvasScene *getCanvasScene();

    QSharedPointer<CanvasContext> getCanvasContext();

    /// @brief 获取图层所属的代理图元
    /// @return
    TransformProxyGraphicsObject *getTransformProxyGraphicsObject();

    /// @brief 获取当前图层所属的资源上下文
    /// @return
    QSharedPointer<SourceProxy> getSourceProxy();

    /// @brief 获取当前图层和子图层的联合区域大小
    /// @return
    QRectF unitedChildsBoundingRect();

    /// @brief 获取当前画板的业务钩子对象(只要是图层对象，都可以获取到业务钩子服务)
    /// @return
    BusinessHooksServer *getBusinessHook();

    GraphicsLayerProperty &getLayerProperty();

    void cleanLayerInfo();

public:
    /// @brief 图元id
    /// @return uuid
    virtual QString id();

    /// @brief 创建内部图元
    virtual void createInternalGraphics();

    /// @brief 更新图元坐标位置,在代理图元大小发生变化的时候需要重新计算
    virtual void updatePosition();

    /// @brief 控制图层的显示和隐藏，图层类必须使用userShow接口，该接口会同时处理Visible的图层属性，而不能使用setvisible
    /// @param visible
    virtual void userShow(bool visible);

    virtual QRectF boundingRect() const override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

protected slots:
    void onPropertyChanged(const QString &name, const QVariant &oldValue, const QVariant &newValue);

protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    virtual void processPropertyChanged(const QString &name, const QVariant &oldValue, const QVariant &newValue);

protected:
    QScopedPointer<GraphicsLayerPrivate> layerData;
};

Q_DECLARE_METATYPE(GraphicsLayer::GraphicsLayerFlag)

#endif // GRAPHICSLAYER_H
