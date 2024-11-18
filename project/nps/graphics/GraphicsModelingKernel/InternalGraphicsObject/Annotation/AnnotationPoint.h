#ifndef ANNOTATIONPOINT_H
#define ANNOTATIONPOINT_H

#include "GraphicsLayer.h"

#include <QObject>
#include <QScopedPointer>

enum annotationChange { startedChange, inChange, finishedChange };

class AnnotationPoint : public GraphicsLayer
{
    Q_OBJECT

public:
    AnnotationPoint(ICanvasScene *canvasScene, int index, QGraphicsItem *parent = 0);
    ~AnnotationPoint();

    static QList<AnnotationPoint *> createAllAnnotationGraphics(ICanvasScene *canvasScene, QPolygonF &cachePoints);

protected:
    virtual QRectF boundingRect() const;

    // virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

signals:

    void positionChanged(QPointF delta, int index, annotationChange state);

public:
    int pointIndex;
    QPointF pressPoint;
};

#endif // ANNOTATIONPOINT_H
