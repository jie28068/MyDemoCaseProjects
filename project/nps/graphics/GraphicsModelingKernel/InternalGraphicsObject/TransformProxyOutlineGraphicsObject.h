#pragma once

#include "GraphicsLayer.h"

/// @brief 代理图层的边框显示类
class TransformProxyOutlineGraphicsObject : public GraphicsLayer
{
    Q_OBJECT
public:
    TransformProxyOutlineGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    virtual ~TransformProxyOutlineGraphicsObject();

    void setShowOutline(bool show);

    virtual void updatePosition();

    virtual QRectF boundingRect() const override;

public slots:
    void onSourceSizeChanged(QString, QVariant);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QRectF proxyRect;
    bool isShowOutline;
};