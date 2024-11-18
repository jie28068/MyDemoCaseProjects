#pragma once

#include <QGraphicsObject>
#include <QSharedPointer>

class PortGraphicsObject;
class IConnectorWireAlgorithm;
class ConnectorWireContext;
class ConnectorWireSwitch;
class ConnectorWireRegulator;
class PortContext;
class ConnectorWireSegment;
class SignalNameGraphicsObject;
class ConnectorWireRegulationController;

class ConnectorWireGraphicsObjectPrivate
{
public:
    ConnectorWireGraphicsObjectPrivate();

    /// @brief 起点图元
    PortGraphicsObject *outPutPortGraphicsObject;
    /// @brief 终点图元
    PortGraphicsObject *inPutPortGraphicsObject;
    /// @brief 连接线上下文信息
    QSharedPointer<ConnectorWireContext> context;
    /// @brief 连接线连接过程中鼠标移动的点坐标
    QPointF movingPos;
    /// @brief 连接线坐标点集合
    QPolygonF points;
    /// @brief 连接线算法
    IConnectorWireAlgorithm *algorithm;
    /// @brief 连接线箭头坐标
    QPolygonF arrowPoints;
    /// @brief 保存连接线上次计算结果连接的起点坐标
    QPointF lastStartPos;
    /// @brief 保存连接线上次计算结果的起点锚点方向
    Qt::AnchorPoint lastStartAnchor;
    /// @brief 保存连接线上次计算结果连接的终点坐标
    QPointF lastEndPos;
    /// @brief 保存连接线上次计算结果的终点锚点方向
    Qt::AnchorPoint lastEndAnchor;

    /// @brief 连接线的创建是通过内存数据load而来
    bool isFromLoad;

    /// @brief 连接线调节器调节之前的坐标点
    QPolygonF oldPoints;
    /// @brief 连接线调节器如果移动了端口,保存移动之前的端口位置
    QSharedPointer<PortContext> oldPortContext;

    /// @brief 高亮同一个端口发出的连接线(和选中颜色一致)
    bool highLightHomologous;

    /// @brief 连接线开关图元
    ConnectorWireSwitch *connectorWireSwitchGraphics;

    /// @brief 该连接线和同一个端口出来的其他连线的分支点
    QList<QPointF> branchPoints;

    /// @brief 已经和该连接线计算过分支点的其他连线uuid,避免重复计算,重新计算连接线分支点之前必须先清除该列表
    QList<QString> branchConnectorWireUUIDList;

    int pathStrokerWidth;

    /// @brief 连接线线段列表
    QList<ConnectorWireSegment *> segments;

    /// @brief 多余的不需要的线段列表
    QList<ConnectorWireSegment *> needlessSegments;

    /// @brief 开始线段
    ConnectorWireSegment *startSegment;

    /// @brief 结束线段
    ConnectorWireSegment *endSegment;

    /// @brief 标记当前连接线包含的线段是否有被选中的
    bool hasSegmentSelected;

    /// @brief 标记连接线是否应该自动调整，在图元移动结束时，重新计算的连接线需要避免和其他连接线重叠
    bool shouldAvoidCollision;

    SignalNameGraphicsObject *signalGraphicsObject;

    ConnectorWireRegulationController *outputController;

    ConnectorWireRegulationController *inputController;
};

ConnectorWireGraphicsObjectPrivate::ConnectorWireGraphicsObjectPrivate()
    : outPutPortGraphicsObject(nullptr),
      inPutPortGraphicsObject(nullptr),
      algorithm(nullptr),
      connectorWireSwitchGraphics(nullptr),
      isFromLoad(false),
      highLightHomologous(false),
      pathStrokerWidth(3),
      hasSegmentSelected(false),
      shouldAvoidCollision(false),
      startSegment(nullptr),
      endSegment(nullptr),
      signalGraphicsObject(nullptr),
      outputController(nullptr),
      inputController(nullptr)
{
}
