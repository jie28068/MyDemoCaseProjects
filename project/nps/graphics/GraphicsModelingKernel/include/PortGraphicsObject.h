#ifndef PORTGRAPHICSOBJECT_H
#define PORTGRAPHICSOBJECT_H

#include "Global.h"
#include "GraphicsLayer.h"

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsObject>
#include <QGraphicsSceneMouseEvent>

class SourceGraphicsObject;
class ConnectorWireGraphicsObject;
class PortContext;
class ICanvasScene;
class PortGraphicsObject;
class PortGraphicsObjectPrivate
{
public:
    PortGraphicsObjectPrivate()
    {
        sourceGraphics = nullptr;
        currentDrawingConnectorWire = nullptr;
        canCreateConnectorWire = false;
        isHover = false;
    }
    /// @brief 资源图元
    SourceGraphicsObject *sourceGraphics;

    /// @brief 端口上下文信息
    QSharedPointer<PortContext> context;

    /// @brief 当前端口正在连接中的连接线对象
    ConnectorWireGraphicsObject *currentDrawingConnectorWire;

    /// @brief 已经连接到该端口的连接线列表
    QList<ConnectorWireGraphicsObject *> linkedConnectorWireGraphicsObject;

    /// @brief 鼠标移动中触碰到的端口图元
    QList<PortGraphicsObject *> movingHoverPortGraphics;

    QSharedPointer<PortContext> oldPortContext;

    /// @brief 是否可以与点击的端口创建连接线
    bool canCreateConnectorWire;

    /// @brief 鼠标是否在端口区域
    bool isHover;
};

class GRAPHICSMODELINGKERNEL_EXPORT PortGraphicsObject : public GraphicsLayer
{
    Q_OBJECT

public:
    friend class TransformProxyGraphicsObject;
    friend class ConnectorWireGraphicsObject;
    friend class ConnectorWirePrompt;

    PortGraphicsObject(ICanvasScene *canvasScene, SourceGraphicsObject *sourceGraphics,
                       QSharedPointer<PortContext> context, QGraphicsItem *parent = 0);
    ~PortGraphicsObject();

public:
    /// @brief 端口上下文信息
    QSharedPointer<PortContext> getPortContext() const;

    /// @brief 端口所属的资源图元对象
    SourceGraphicsObject *getSourceGraphics();

    /// @brief 当前端口和连接线连接
    /// @param connectorWire 连接线对象
    void linkConnectorWire(ConnectorWireGraphicsObject *connectorWire);

    /// @brief 当前端口和连接线分离
    /// @param connector 连接线对象
    virtual void unLinkConnectorWire(ConnectorWireGraphicsObject *connectorWire);

    /// @brief 获取和该端口图元建立连接的连接线列表
    QList<ConnectorWireGraphicsObject *> getLinkedConnectorWireList();

    /// @brief 当前端口是否可以连接
    /// @return
    bool canLink();

    /// @brief 计算该端口的连接线的分支点
    void calcuteConnectorWireBranchPoints();

    /// @brief 清理端口连接线的分支点坐标
    void cleanConnectorWireBranchPoints();

    /// @brief 检测鼠标左键按下并在移动中的端口图元，命中则置高亮
    void checkAvaliablePort(QPointF scenePos);

    /// @brief 高亮端口
    void lightMovingHoverPortGraphics(QList<PortGraphicsObject *> portGraphics = QList<PortGraphicsObject *>());

    /// @brief 当前端口是否是输出端口
    /// @return
    bool isOutputPort();

    /// @brief 判断当前端口是否能与场景中被点击了的端口相连
    /// @return
    bool isCanLinkWithClickedPort();

    /// @brief 鼠标是否悬浮在端口上
    /// @return
    bool isMouseHover();

public:
    /// @brief 图元类型
    virtual int type() const override;

    /// @brief 图元id
    virtual QString id() override;

    /// @brief 更新坐标
    virtual void updatePosition() override;

    /// @brief 获取端口图元在场景内的中心点坐标
    virtual QPointF getLinkPos();

    /// @brief 获取端口中心点坐标
    virtual QPointF getCenterPos();

    /// @brief 获取端口图元中心点坐标的外延点坐标
    virtual QPointF getExternalPos();

    /// @brief 获取端口的锚点方向,这个会影响端口外延点的计算
    virtual Qt::AnchorPoint getAnchorPoint();

    /// @brief 端口连接改变
    virtual void linkChanged();

    /// @brief 端口所连接的连接线自动布局
    virtual void autoLayout();

    /// @brief 端口名称区域大小
    /// @return
    virtual QRectF portNameRect();

    /// @brief 重置端口锚点方向
    virtual void resetPortAnchor();

    virtual QRectF boundingRect() const;

    /// @brief 端口是否可以移动
    /// @return
    virtual bool canMove();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    virtual void keyPressEvent(QKeyEvent *event);

    void setTransFormCommand(QList<QTransform> &oldTransFormList, QList<QTransform> &newTransFormList);

private:
    // 是否需要删除端口图元自身
    void deleteGraphicsObjectSelf();

    // 删除端口图元
    void deletePortGraphicsObjectSelf();

    /// @brief 是否是被删除的模块
    bool isBeDeletedModule(QGraphicsItem *item);

    /// @brief 校验端口port是否和当前端口可以匹配自动连接规则
    /// @param port 被匹配端口
    /// @return
    bool portIsMatchAutoConnect(PortGraphicsObject *port);

    /// @brief 校验端口的锚点方向是否匹配，必须是 右-左  上-下 这种
    /// @param port
    /// @return
    bool portAnchorIsMatchAutoConnect(PortGraphicsObject *port);

    /// @brief 校验端口的类型是否匹配，必须分别为输入和输出类型
    /// @param port
    /// @return
    bool portTypeIsMatchAutoConnect(PortGraphicsObject *port);

    /// @brief 获取提示端口连接的连接线坐标点
    /// @return
    QPointF getPropmtPos();

public slots:
    void onPortChanged();

protected:
    QScopedPointer<PortGraphicsObjectPrivate> dataPtr;
};

#endif // PORTGRAPHICSOBJECT_H
