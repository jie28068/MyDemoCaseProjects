#pragma once

#include "GraphicsLayer.h"

class MathMLGrapihcsObjectPrivate;
class GRAPHICSMODELINGKERNEL_EXPORT MathMLGraphicsObject : public GraphicsLayer
{
public:
    MathMLGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    virtual ~MathMLGraphicsObject();

    /// @brief 根据名称设置数学公式
    /// @param name 和数学公式绑定的名称
    void setMathMLName(const QString &name);

    virtual void updatePosition();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    virtual QRectF boundingRect() const override;

private:
    QScopedPointer<MathMLGrapihcsObjectPrivate> dataPtr;
};
