#pragma once
#include "Global.h"

#include "CanvasContext.h"
#include <QDateTime>
#include <QDebug>
#include <QGraphicsItem>
#include <QTransform>

class SourceGraphicsObject;
class ConnectorWireContext;

class GRAPHICSMODELINGKERNEL_EXPORT Utility
{
public:
    enum WireSegmentDirection {
        kInvalid,
        kLeftToRight,
        kRightToLeft,
        kTopToBottom,
        kBottomToTop,
    };

    /// @brief 计算QTransform矩阵角度
    /// @param tranfrom 旋转矩阵
    /// @return 角度值
    static int getTransformAngle(const QTransform &tranfrom);

    /// @brief 原始锚点方向和旋转角度叠加后的实际锚点方向
    /// @param anchor 原始锚点方向
    /// @param angle 旋转角度
    /// @return
    static Qt::AnchorPoint getAnchorPoint(Qt::AnchorPoint anchor, int angle);

    /// @brief 判断点pos是否在startPos和endPos所连接的线段上
    /// @param pos 点坐标
    /// @param startPos 线段起点
    /// @param endPos 线段终点
    /// @return
    static bool pointIsInLine(QPoint pos, QPoint startPos, QPoint endPos);

    /// @brief 检测startPos和endPos的连线和给定的矩形区域列表collisionRects是否有重合
    /// @param startPos
    /// @param endPos
    /// @param collisionRects
    /// @return
    static bool checkCollision(QPointF startPos, QPointF endPos, QList<QRectF> collisionRects);

    static bool checkCollision(QPointF startPos, QPointF endPos, const QPainterPath &path);

    /// @brief 计算连接点集合的连线的直角数
    /// @param points 坐标点集合
    /// @return 返回直角数
    static int getRightAngleCounts(QPolygonF points);

    /// @brief 生成UUID
    /// @return
    static QString createUUID();

    /// @brief 坐标点对齐网格
    /// @param pos 未对齐之前的坐标
    /// @param gridSpace 网格间距
    /// @return 对齐网格之后的坐标
    static QPointF pointAlignmentToGrid(QPointF pos, int gridSpace);

    static QSizeF sizeAlignmentToGrid(QSizeF size, int gridSpace);

    /// @brief 矩形区域对齐网格
    /// @param rect 原矩形区域
    /// @param gridSpace 网格间距
    /// @return 对齐网格后的矩形区域
    static QRectF rectAlignmentToGrid(QRectF rect, int gridSpace);

    static qreal numberAlignmentHalfGrid(qreal value, qreal gridSpace);

    static QPointF calculateAlignmentPos(SourceGraphicsObject *sourceGraphics, QPointF pos);

    /// @brief double和0比较，是否等于0
    /// @param value
    /// @return
    static bool isEqualZero(qreal value);

    static void qt_graphicsItem_shapeFromPath(QPainterPath &newpath, const QPainterPath &path, const QPen &pen);

    /// @brief 图元围绕中心点旋转
    /// @param item 被旋转的图元
    /// @param angle 角度
    static void rotateOnCenter(QGraphicsItem *item, int angle);

    /// @brief 获取int类型数据中二进制位为1的索引列表,索引从0开始
    /// @param value
    /// @return 返回二进制为1的位置索引列表
    static QList<int> getBinaryOneFlag(int value);

    static QString getRandomString(int len);

    /// @brief 获取angle对应索引
    /// @param 角度angle
    /// @return 返回int型角度索引
    static int getAngleIndex(int angle);

    static QPointF calcBusRegulatorPos(QPolygonF points, int angle);

    static Qt::AlignmentFlag transAlignment(int alignment);

    /// @brief 给QPolygonF内的坐标点添加偏移量
    static void polygonfAddOffset(QPolygonF &points, QPointF offset);

    static const char *QStringToChar(const QString &str);

    static bool isContainByUUID(QList<QSharedPointer<ConnectorWireContext>> wires,
                                QSharedPointer<ConnectorWireContext> wire);

    /// @brief 两点组成的线段角度
    /// @param p1
    /// @param p2
    /// @return
    static qreal lineAngle(QPoint p1, QPoint p2);

    /// @brief 两点组成的线段长度
    /// @return
    static qreal lineDistance(QPointF p1, QPointF p2);

    static QPointF adjustItemChangePos(QRectF sceneRect, QRectF changingRect, QPointF alignMovingPos, QPointF pos);

    /// @brief 计算坐标点连线的长度
    /// @return
    static qreal getPolygonLineLength(QPolygon polygons);

    static QPointF calcAnglePoint(int angle, QRectF rc);

    static QPolygonF getArrorwPoint(QPointF startPoint, QPointF endPoint, double angle, qreal length);

    static void filterSamePoints(QPolygonF &points);

    static CanvasContext::Type getCanvasTypeByModelType(int boardModelType);
    static int getModelTypeByCanvasType(CanvasContext::Type canvaType);

    static WireSegmentDirection getSegmentDirection(QPoint startPt, QPoint endPt);

    static QPoint getBranchPoint(QPolygonF points1, QPolygonF points2, bool filter = true);
};

namespace nps {

namespace helper {

template<typename F>
struct Defer {
    typename std::remove_reference<F>::type _f;
    Defer(F &&f) noexcept : _f(std::forward<F>(f)) { }
    ~Defer() { _f(); }
};

template<typename F>
inline Defer<F> make_defer(F &&f) noexcept
{
    return Defer<F>(std::forward<F>(f));
}

#define NPS_CONCAT(x, n) x##n
#define NPS_MAKE_TEMP_NAME(x, n) NPS_CONCAT(x, n)
#define NPS_DEFER_NAME NPS_MAKE_TEMP_NAME(nps_defer_, __LINE__)

}

}

#define NPS_DEFER(e) auto NPS_DEFER_NAME = nps::helper::make_defer([&]() { e; });

class CostTimeHelper
{
public:
    CostTimeHelper(const QString &title) : msgTitle(title) { startTime = QDateTime::currentDateTime(); };

    ~CostTimeHelper()
    {
        QDateTime endTime = QDateTime::currentDateTime();
        qint64 cost_time = startTime.msecsTo(endTime);
        qDebug() << msgTitle << " cost time:" << cost_time << "ms";
    }

private:
    QString msgTitle;
    QDateTime startTime;
};

#define DEBUG_COST_TIME() auto NPS_MAKE_TEMP_NAME(cost_time_, __LINE__) = CostTimeHelper(__FUNCTION__)
#define DEBUG_COST_TIME_WITH_MSG(msg) auto NPS_MAKE_TEMP_NAME(cost_time_, __LINE__) = CostTimeHelper(msg)