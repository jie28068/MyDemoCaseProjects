#ifndef SOURCEGRAPHICSOBJECT_H
#define SOURCEGRAPHICSOBJECT_H

#include "Global.h"
#include "GraphicsLayer.h"
#include "PortContext.h"
#include "PortGraphicsObject.h"

#include <QGraphicsObject>
#include <QMenu>

class SourceProxy;
class ICanvasScene;

class SourceGraphicsObjectPrivate;
class GRAPHICSMODELINGKERNEL_EXPORT SourceGraphicsObject : public GraphicsLayer
{
    Q_OBJECT

public:
    SourceGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source, QGraphicsItem *parent = 0);
    ~SourceGraphicsObject();

    /// @brief 支持的鼠标缩放拖动方向
    enum ScaleDirection { kScaleNone = 0, kScaleHorizontal = 1 << 0, kScaleVertical = 1 << 1, kScaleAll = 1 << 2 };

    enum AlignmentType {
        kAlignmentNone = 0, // 不对齐
        kAlignmentGrid,     // 网格对齐
        kAlignmentHalfGrid  // 一半网格间距对齐
    };

public:
    /// @brief 在指定坐标点添加端口图元
    /// @param scenePos 场景坐标
    /// @return
    PortGraphicsObject *addPortGraphicsObject(QPointF scenePos);

    /// @brief 根据端口上下文信息添加端口图元
    /// @param portContext 端口上下文信息
    /// @return
    PortGraphicsObject *addPortGraphicsObject(PPortContext portContext);

    /// @brief 删除端口
    /// @param pPortGraphicsObject 端口图元
    void deletePortGraphicsObject(PortGraphicsObject *pPortGraphicsObject);

    void deletePortGraphics(PortGraphicsObject *pPortGraphicsObject); // 删除控制类型端口

    /// @brief 获取当前图元在场景坐标系的区域
    /// @return
    QRectF getSourceSceneRect();

    /// @brief 获取资源图元所描述的SourceProxy对象
    /// @return
    QSharedPointer<SourceProxy> getSourceProxy() const;

    /// @brief 计算缩放、旋转、平移变换之后的boundingrect
    QRectF getTransformBoundingRect() const;

    /// @brief 应用transform
    void applyTransform();

    /// @brief 设置资源支持的鼠标拖动缩放类型
    /// @param direction
    void setScaleDirection(SourceGraphicsObject::ScaleDirection direction);

    /// @brief 资源支持的鼠标拖动缩放类型
    SourceGraphicsObject::ScaleDirection getScaleDirection();

    /// @brief 获取x轴对齐规则，默认按照网格对齐
    AlignmentType getXAxisAlignment();

    void setXAxisAlignment(SourceGraphicsObject::AlignmentType type);

    /// @brief 获取y轴对齐规则，默认按照网格对齐
    AlignmentType getYAxisAlignment();

    /// @brief 设置y轴对齐规则，默认按照网格对齐
    /// @param type
    void setYAxisAlignment(SourceGraphicsObject::AlignmentType type);

    /// @brief 调整图元大小，但是不会改变sourceProxy内部保存的size值，注意和setSourceBoundingRect区分使用
    /// @param rc
    void adjustBoundingRect(QRectF rc);

    /// @brief 获取当前资源图层是否是鼠标悬浮状态
    /// @return
    bool isHoverStatus();

    /// @brief 设置资源图层鼠标悬浮状态
    /// @param status
    void setHoverStatus(bool status);

public:
    virtual int type() const override;

    virtual QString id() override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    virtual QRectF boundingRect() const;

    /// @brief 用户自定义菜单
    virtual void userMenu(QSharedPointer<QMenu> menu);

    virtual QSizeF getMinimumSize() const;

    /// @brief 设置图元边界区域，该接口会更新对应SourceProxy的size值，注意和adjustBoundingRect区分使用
    /// @param rect
    virtual void setSourceBoundingRect(QRectF rect = QRectF()); // 设置图元边界

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    bool checkIsHaveConnectorWire(); // 检查图层端口是否连有连接线

private slots:
    void viewDrawingBoard();
    void onOpenSourceProperty();

    void onAddPort(PPortContext portContext);

    void onDeletePort(PPortContext portContext);

    virtual void onSourcePropertyChanged(QString key, QVariant oldValue, QVariant newValue);

protected:
    QScopedPointer<SourceGraphicsObjectPrivate> dataPtr;
};

typedef QSharedPointer<SourceGraphicsObject> PSourceGraphicsObject;

#endif // SOURCEGRAPHICSOBJECT_H
