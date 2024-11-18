#ifndef CONNECTORWIREGRAPHICSOBJECT_H
#define CONNECTORWIREGRAPHICSOBJECT_H

#include "Global.h"
#include "GraphicsLayer.h"
#include "IConnectorWireAlgorithm.h"
#include "Utility.h"

#include <QGraphicsObject>

class PortGraphicsObject;
class IConnectorWireAlgorithm;
class ConnectorWireContext;
class ConnectorWireSwitch;
class ConnectorWireRegulator;
class ConnectorWireSegment;
class ManhattaConnectorWireAlgorithm;
class SignalNameGraphicsObject;
class ConnectorWireRegulationController;

class ConnectorWireGraphicsObjectPrivate;
class GRAPHICSMODELINGKERNEL_EXPORT ConnectorWireGraphicsObject : public GraphicsLayer
{
    Q_OBJECT

    friend class PortGraphicsObject;
    friend class CanvasSceneDefaultImpl;
    friend class ConnectorWireSegment;
    friend class ManhattaConnectorWireAlgorithm;
    friend class TransformProxyGraphicsObject;
    friend class ConnectorWireRegulationController;

public:
    ConnectorWireGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    ~ConnectorWireGraphicsObject();

public:
    /// @brief 连接线连接到图元端口
    /// @param portGraphics 图元端口
    /// @return
    bool linkPortGraphicsObject(PortGraphicsObject *portGraphics);

    /// @brief 连接线断开和端口的连接
    bool unlinkPortGraphicsObject(PortGraphicsObject *portGraphics);

    /// @brief 清理连接线
    void clearLinks();

    /// @brief 设置鼠标移动点坐标
    /// @param pos 鼠标的场景坐标
    void setMovingPos(QPointF pos);

    /// @brief 连接线两端是否都连接到了端口图元
    /// @return
    bool isConnectedWithPort();

    /// @brief 设置连接线算法
    /// @param algoritmName 算法名称
    void setAlogrithm(QString algoritmName);

    /// @brief 获取连接线算法名称
    /// @return
    QString getAlgorithmName();

    /// @brief 从连接线上下文信息，自动连接画板上的端口
    /// @param context
    /// @return
    bool loadFromContext(QSharedPointer<ConnectorWireContext> context);

    /// @brief 获取当前连接线的连接线信息上下文
    /// @return
    QSharedPointer<ConnectorWireContext> getConnectorWireContext();

    /// @brief 获取当前连接线刷新参数
    /// @return
    QSharedPointer<WireParameters> refreshWireParameters();

    /// @brief 连接线开关切换
    /// @param value
    void doSwitch(bool value);

    /// @brief 获取和port端口连接的另外一个端口
    /// @param port
    /// @return
    PortGraphicsObject *getAnotherPort(PortGraphicsObject *port);

    /// @brief 获取连接线的点坐标
    QPolygonF getPoints();

    void setPoints(const QPolygonF &points);

    /// @brief 缓存连接点数据到连接线上下文信息
    void cachePoints();

    /// @brief 清理分支点坐标
    void cleanBranchPoints();

    void addBranchPoint(const QPointF &pt);

    /// @brief 获取连接线的输出端口
    PortGraphicsObject *getOutputTypePortGraphics();

    /// @brief 获取连接线的输入端口
    /// @return 输入端口图元
    PortGraphicsObject *getInputTypePortGraphics();

    /// @brief 获取开始端口图元
    /// @return
    PortGraphicsObject *getStartPortGraphics();

    /// @brief 获取结束端口图元
    /// @return
    PortGraphicsObject *getEndPortGraphics();

    /// @brief 返回连接线的开关图元,用于切换连接线的连接状态(连通、端口)
    /// @return 目前只有电气画板的连接线才有开关控制，其他类型画板返回nullptr
    ConnectorWireSwitch *getConnectorWireSwitch();

    /// @brief 连接线两端的资源是否都是选中状态
    /// @return
    bool isSourceBothSelected();

    /// @brief 设置连接线是否高亮显示同源连接线
    void enableHighLightHomologous(bool show);

    /// @brief 连接线是否高亮显示同源连接线
    /// @return
    bool isHighLightHomologous();

    void applyOffsetPos();

    /// @brief 端口锚点坐标是否改变
    bool isAnchor();

    void setArrowPoints(QPolygonF &points);

public:
    virtual int type() const override;

    virtual QString id() override;

    virtual QRectF boundingRect() const;

protected:
    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    virtual QPainterPath shape() const override;

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void setAutoCalculation(bool flag);

    /// @brief 调整连接线计算shape时，连接线两端的端口检测区域，
    /// 解决bug: 如果输出端口存在连接线，那么在输出端口边缘的时候使用鼠标点击创建新连接线的时候，
    /// 很大几率会点击到已经存在的连接线，导致交互性变差
    /// @return
    static QPointF adjustShapePoint(Qt::AnchorPoint anchor, QPointF pos, int offset);

signals:

public slots:
    /// @brief 连接线算法改变响应
    /// @param name 连接线算法名称
    void onConnectorWireAlgorithmChanged(QString key, QVariant value);

    /// @brief 连接线开关状态变化
    void onSwitchStatusChanged(bool isOn);

    void onSegmentPositionChanged(QPointF pt);

    void onSegmentSelectedChanged(bool selected);

    void onTransformProxyChanged();

    void onSignalNameChanged(QString name);

    void onPointsUpdated(QPolygonF pts);

    void onRegulationControllerPositionChanged(const QPointF &position);

private:
    int indexOfSegment(ConnectorWireSegment *segment);

    bool hasSegmentSelected();

    void processStartOrEndLineSegment(ConnectorWireSegment *segment);

    void syncHighLightHomologousStatus();

    /// @brief 连接线的坐标点符合算法特性  目前只校验manhatan算法
    bool isPointsMatchManhattaAlgorithm();

    /// @brief 获取信号名称显示的位置坐标
    QPointF getSignalNamePos(bool &isVertical);

    bool shouldAvoidCollision();

    /// @brief 折线算法fix
    void manhattanWireFixPatch();

    /// @brief 折线算法保存坐标点时，生成线段组
    void manhattanWireCachePoints();

    /// @brief 调整连接线开关图元位置
    void adjustWireSwitchPos();

    /// @brief 调整连接线信号名称图元位置
    void adjustWireSignalNamePos();

    Utility::WireSegmentDirection getRegulationControllerDirection(ConnectorWireRegulationController *controller);

    ConnectorWireRegulationController *getConnectorWireRegulationController(QString portType);

private:
    QScopedPointer<ConnectorWireGraphicsObjectPrivate> dataPtr;
};

#endif // CONNECTORWIREGRAPHICSOBJECT_H
