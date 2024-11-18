#ifndef ANNOTATIONSOURCEGRAPHICSOBJECT_H
#define ANNOTATIONSOURCEGRAPHICSOBJECT_H

#include "ICanvasScene.h"
#include "SourceGraphicsObject.h"
#include <QObject>

class AnnotationSourceGraphicsObject : public SourceGraphicsObject
{
    Q_OBJECT
public:
    enum AnnotationType {
        kNoneAnnotation,            // 非法注解类型
        kCircleAnnotation,          // 椭圆形注解
        kRectAnnotation,            // 矩形注解
        kLineSegment,               // 线段
        kLineSegmentArrow,          // 带箭头的线段注解
        kBrokenLineAnnotation,      // 折线注解
        kBrokenLineArrowAnnotation, // 带箭头的折线注解
        kPolygonAnnotation,         // 多边形
        kTextAnnotation,            // 文字注解
        kImageAnnotation,           // 图片注解
        kArcAnnotation,             // 圆弧
    };

    enum Status {
        kStatusStarted = 1 << 0, // 交互中,
        kStatusCancled = 1 << 1, // 取消
        kStatusFinished = 1 << 2 // 完成
    };

    /// @brief 处理鼠标左键点击事件
    virtual void processMousePressEvent(QMouseEvent *event);

    /// @brief 处理鼠标移动事件
    virtual void processMouseMoveEvent(QMouseEvent *event);

    /// @brief 处理鼠标释放事件
    virtual void processMouseReleaseEvent(QMouseEvent *event);

    /// @brief 处理双击事件
    virtual void processMouseDoubleClickEvent(QMouseEvent *event);

    virtual void setMovingPos(const QPointF &scenePos);

    virtual void createInternalGraphics();

    AnnotationType getAnnotationType() const;

    void setStatus(Status s);

    bool isInterfactionFinished();

    virtual void userShow(bool visible);

protected:
    /// @brief 设置坐标点几何
    /// @param points
    void setAnnotationPoints(QPolygonF points);
    /// @brief 获取坐标点几何
    /// @return
    QPolygon getAnnotationPoints();

    /// @brief 获取几何图形线宽
    /// @return
    double getAnnotationLineWidth() const;
    /// @brief 设置几何图形线宽
    /// @param width
    void setAnnotationLineWidth(double width);

    /// @brief 获取线段样式
    Qt::PenStyle getSegmentStyle() const;
    /// @brief 获取是否显示前端箭头
    bool getShowFrontArrow() const;
    /// @brief 获取是否显示后端箭头
    bool getShowEndArrow() const;
    /// @brief 获取文本内容
    QString getContent() const;
    /// @brief 获取文本字体样式
    QFont getFont() const;
    /// @brief 获取线段或者边框颜色
    QColor getSegmentColor() const;

    QPointF getMouseEventScenePos(QMouseEvent *event);

    virtual void initProperty();

signals:
    /// @brief 注解图元交互状态结束
    void interactionFinished();

    /// @brief 注解图元被取消
    void cancled();

public slots:
    void onRefreshAnnotationType();

protected:
    AnnotationSourceGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                   QGraphicsItem *parent = 0);
    ~AnnotationSourceGraphicsObject();

protected:
    Status status;

    AnnotationType annotationType;

    /// @brief points保存坐标点信息
    QPolygonF cachePoints;
};

#endif // ANNOTATIONSOURCEGRAPHICSOBJECT_H
