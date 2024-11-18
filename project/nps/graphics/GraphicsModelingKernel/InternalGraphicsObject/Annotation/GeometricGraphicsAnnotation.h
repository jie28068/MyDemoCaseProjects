#ifndef GEOMETRICGRAPHICSANNOTATION_H
#define GEOMETRICGRAPHICSANNOTATION_H

/*
几何图形注解，包含矩形、圆、线段、折线、多边形
*/

#include "AnnotationPoint.h"
#include "AnnotationSourceGraphicsObject.h"

class GeometricGraphicsAnnotation : public AnnotationSourceGraphicsObject
{
    Q_OBJECT

public:
    GeometricGraphicsAnnotation(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                QGraphicsItem *parent = 0);
    ~GeometricGraphicsAnnotation();

    virtual int type() const override;

    /// @brief 处理鼠标点击事件
    virtual void processMousePressEvent(QMouseEvent *event) override;

    /// @brief 处理鼠标移动事件
    virtual void processMouseMoveEvent(QMouseEvent *event) override;

    /// @brief 处理鼠标释放事件
    virtual void processMouseReleaseEvent(QMouseEvent *event) override;

    virtual void processMouseDoubleClickEvent(QMouseEvent *event) override;

    QPolygonF getCachePoints();

    virtual void createInternalGraphics();

    void refreshAnnotationPoint(bool isSelected = false);

public:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    virtual QRectF boundingRect() const;

    virtual QPainterPath shape() const;

public slots:

    void onPointChange(QPointF delta, int index, annotationChange state);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void drawFrontArrow(QPainter *painter, QLineF &line);

    void drawEndArrow(QPainter *painter, QLineF &line);

    // 绘制完成后校验点之间的距离 如果太短本次绘制无效
    bool IsVailid();

    void interactionFinish();

    virtual void initProperty();

    // 矩形和圆只存了两个点 数据需要补到四个点
    void completePoint(QPolygonF &pointArray);

    void drawPointsRect(QPainter *painter);

    void changeCachePoints(QPointF delta, int index);

    static QRectF arcCalcRect(QPointF centerPos, QPointF endPos);

    QPainterPath getArcPath(QPointF point1, QPointF point2, QPointF point3);

private:
    /// @brief 保存鼠标移动中的坐标点，用于交互式绘制中的状态
    QPointF movingPoint;

    QList<AnnotationPoint *> annotationPointGraphics;

    QPolygonF startChangePoints;
};

#endif // GEOMETRICGRAPHICSANNOTATION_H
