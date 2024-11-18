#ifndef GOTOFROMSUMSOURCEGRAPHICSOBJECT_H
#define GOTOFROMSUMSOURCEGRAPHICSOBJECT_H

#include "ControlModuleSourceGraphicsObject.h"
#include "defines.h"

/// @brief goto和from模块类
class GoToFromSourceGraphicsObjct : public ControlModuleSourceGraphicsObject
{
    Q_OBJECT

public:
    GoToFromSourceGraphicsObjct(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                QGraphicsItem *parent = 0);
    virtual ~GoToFromSourceGraphicsObjct();

    virtual void createInternalGraphics();

    virtual void setSourceBoundingRect(QRectF rect = QRectF());

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    virtual bool otherPaint(QPainter *painter, const QRectF rect);

private:
    /// @brief 组成goto from模块形状的点
    QPolygonF shapePoints;

private:
};

#endif // GOTOFROMSUMSOURCEGRAPHICSOBJECT_H