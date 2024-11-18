#pragma once
#include <QObject>
#include <QPainterPath>
#include <QPointF>
#include <QPolygonF>
#include <QRectF>

#define ManhattaAlgorithmName "Manhatta"
#define StraightLineAlgorithmName "StraightLine"
#define BezierAlogrithmName "Bezier"

class PortGraphicsObject;

struct WireParameters {
    /// @brief 连接线UUID
    QString wireUUID;
    /// @brief 起始点坐标
    QPointF startPos;
    /// @brief 结束点坐标
    QPointF endPos;
    /// @brief 起始点外延点坐标
    QPointF startExternalPos;
    /// @brief 结束点外延点坐标
    QPointF endExternalPos;
    /// @brief 起始点锚点方向
    Qt::AnchorPoint startAnchor;
    /// @brief 结束点锚点方向
    Qt::AnchorPoint endAnchor;
    /// @brief 障碍区域
    char *obstacleArea;
    /// @brief
    bool autoCalculate;
    /// @brief 计算的路径点集合
    QPolygonF pathResult;
    ///
    QPainterPath obstacleAreaPath;

    /// @brief 标记是否是起始点移动
    bool isStartMoving;

    /// @brief 标记是否是结束点移动
    bool isEndMoving;

    QString algorithmName;

    WireParameters()
    {
        wireUUID = "";
        isStartMoving = false;
        isEndMoving = false;
        autoCalculate = true;
        algorithmName = ManhattaAlgorithmName;
    };
};

class IConnectorWireAlgorithm : public QObject
{
    Q_OBJECT
public:
    IConnectorWireAlgorithm(QObject *parent = 0) : QObject(parent) {};

    virtual bool portMoving(QPointF movingPos, int movingPortType, QPolygonF &points) { return false; }

    /// @brief 获取算法名称
    /// @return
    virtual QString getAlgorithmName() = 0;

    virtual QPolygonF getAlgorithmPath(QPointF startPos, QPointF startExternalPos, QPointF finishPos,
                                       QPointF finishExternalPos, QPainterPath obstaclePath = QPainterPath())
    {
        return QPolygonF();
    }

    Qt::AnchorPoint getAnchorPoint(QPoint &startPos, QPoint &endPos)
    {
        if (startPos.x() == endPos.x()) {
            if (startPos.y() <= endPos.y()) {
                return Qt::AnchorBottom;

            } else {
                return Qt::AnchorTop;
            }
        } else if (startPos.y() == endPos.y()) {
            if (startPos.x() <= endPos.x()) {
                return Qt::AnchorRight;
            } else {
                return Qt::AnchorLeft;
            }
        }
        return Qt::AnchorHorizontalCenter;
    }

signals:
    /// @brief 连接线的坐标点集合变更
    void pointsChanged(QPolygonF points);
};
