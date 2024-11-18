#pragma once

#include "GraphicsLayer.h"

#include <QPixmap>

class TransformProxyGraphicsObject;

class ImageGraphicsObject : public GraphicsLayer
{
    Q_OBJECT
public:
    enum ImageType { kBackground, kForeground };

    ImageGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent = 0);
    virtual ~ImageGraphicsObject();

    virtual QRectF boundingRect() const;

    bool loadFromData(const QByteArray &data);

    void setFillColor(QColor color);

    void setImageType(ImageType type);

    virtual void updatePosition();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

public slots:
    void onSourcePropertyChanged(const QString &name, const QVariant &oldValue, const QVariant &newValue);

private:
    TransformProxyGraphicsObject *proxyGraphicsObject;

    QPixmap imagePixmap;

    QColor fillColor;

    ImageType imageType;
};