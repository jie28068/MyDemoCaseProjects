#pragma once

#include <QGraphicsItemGroup>
#include <QRect>

class TransformItemGroupPrivate;
class ICanvasScene;
class ConnectorWireGraphicsObject;
class TransformProxyGraphicsObject;

class TransformItemGroup : public QGraphicsItemGroup
{
public:
    TransformItemGroup(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);

    void clear();

    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    virtual QRectF boundingRect() const;

    virtual QPainterPath shape() const;

    void resetBoundingRect();

    void addToGroup(QGraphicsItem *item);

    void removeFromGroup(QGraphicsItem *item);

    bool hasLayer(QString id);

    QList<ConnectorWireGraphicsObject *> getConnectorWires();

    QList<TransformProxyGraphicsObject *> getTransformProxyGraphics();

    // 生成对齐线
    void createAlignLine();

    // 图元组中位于鼠标位置下的最上层图元
    void setUnderMouseProxy();

    void clearUnderMouseProxy();

    /// @brief 端口是否存在连接线
    /// @return
    bool isExistConnectorWires();

    /// @brief 是否存在系统本地代码型
    /// @return
    bool isExistControlCode();

    /// @brief 处理undo redo
    /// @return
    void dealUndoRedo(QPointF offset);

    void removeConnectorWire(ConnectorWireGraphicsObject *connector);

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    void onTriggerCopy();
    void addThroughAndDisableAction(QSharedPointer<QMenu> menu);

    QScopedPointer<TransformItemGroupPrivate> dataPtr;
};
