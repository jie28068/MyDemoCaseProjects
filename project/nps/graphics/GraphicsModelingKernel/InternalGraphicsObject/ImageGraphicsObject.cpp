#include "ImageGraphicsObject.h"
#include "CanvasSceneDefaultImpl.h"
#include "SourceProxy.h"
#include "TransformProxyGraphicsObject.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>

ImageGraphicsObject::ImageGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent)
{
    proxyGraphicsObject = getTransformProxyGraphicsObject();
    if (proxyGraphicsObject) {
        PSourceProxy sourceProxy = proxyGraphicsObject->getSourceProxy();
        SourceProperty &sourceProperty = sourceProxy->getSourceProperty();
        connect(&sourceProperty, &NPSPropertyManager::propertyChanged, this,
                &ImageGraphicsObject::onSourcePropertyChanged);
    }
    fillColor = QColor();
    QGraphicsDropShadowEffect *e = new QGraphicsDropShadowEffect(this);
    e->setOffset(2, 2);
    e->setBlurRadius(4);
    e->setColor(QColor(25, 25, 25, 150));
    setGraphicsEffect(e);
}

ImageGraphicsObject::~ImageGraphicsObject() { }

QRectF ImageGraphicsObject::boundingRect() const
{
    if (proxyGraphicsObject) {
        return proxyGraphicsObject->boundingRect();
    }
    return QRectF();
}

bool ImageGraphicsObject::loadFromData(const QByteArray &data)
{
    return imagePixmap.loadFromData(data);
}

void ImageGraphicsObject::setFillColor(QColor color)
{
    fillColor = color;
}

void ImageGraphicsObject::setImageType(ImageType type)
{
    imageType = type;
    if (imageType == kBackground) {
        setLayerFlag(GraphicsLayer::kBackgroundLayer);
    } else if (imageType == kForeground) {
        setLayerFlag(GraphicsLayer::kForegroundLayer);
    }
}

void ImageGraphicsObject::updatePosition()
{
    update();
}

void ImageGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    PSourceProxy sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    if (sourceProxy->moduleType() == GKD::SOURCE_MODULETYPE_ANNOTATION) {
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, false);

    QRect rc = boundingRect().toRect().adjusted(10, 10, -10, -10);
    auto source = getSourceProxy();
    QColor highLightColor = getCanvasContext()->getCanvasProperty().getHighLightColor();
    if (source && source->moduleType() == GKD::SOURCE_MODULETYPE_CONTROL
        && (source->state() == "through" || source->state() == "disable")) {
        // painter->setOpacity(0.3);
        // fillColor.setAlpha(64);
        // highLightColor.setAlpha(64);
    }

    if (fillColor.isValid()) {
        painter->fillRect(boundingRect(), QBrush(fillColor));
    }

    if (!imagePixmap.isNull()) {

        if (rc.width() > imagePixmap.width() && rc.height() > imagePixmap.height()) {
            // 当前大小足以容纳图片，把图片居中显示
            int xOffset = (rc.width() - imagePixmap.width()) / 2;
            int yOffset = (rc.height() - imagePixmap.height()) / 2;
            QPoint topLeft = rc.topLeft() + QPoint(xOffset, yOffset);
            QRect dstRect = QRect(topLeft, imagePixmap.size());
            painter->drawPixmap(dstRect, imagePixmap);
        } else {
            qreal pixmapRatio = imagePixmap.width() * 1.0 / imagePixmap.height();
            qreal graphicsRatio = rc.width() * 1.0 / rc.height();
            if (pixmapRatio > graphicsRatio) {
                qreal graphicsHeight = rc.width() / pixmapRatio;
                QRectF dstRect = QRectF(QPoint(0, 0), QSize(rc.width(), graphicsHeight));
                dstRect.moveCenter(rc.center());
                painter->drawPixmap(dstRect.toRect(), imagePixmap);
            } else {
                qreal graphicsWidth = rc.height() * pixmapRatio;
                QRectF dstRect = QRectF(QPoint(0, 0), QSize(graphicsWidth, rc.height()));
                dstRect.moveCenter(rc.center());
                painter->drawPixmap(dstRect.toRect(), imagePixmap);
            }
        }
    }

    SourceProperty &sourceProperty = sourceProxy->getSourceProperty();

    if (imageType == kBackground) {
        QPen pen;
        pen.setWidthF(1);
        pen.setColor(sourceProxy->getStateColor());
        pen.setCosmetic(true);

        if (proxyGraphicsObject) {
            bool isHighLight = proxyGraphicsObject->getLayerProperty().getHightLight();
            if (isHighLight) {
                pen.setColor(highLightColor);
                if (sourceProxy->moduleType() == "control") {
                    painter->fillRect(boundingRect(), QBrush(highLightColor));
                }
            }
        }

        bool showOutsideBorder = sourceProperty.isShowOutSideBorder();
        if (showOutsideBorder) {
            painter->setPen(pen);
            painter->drawRect(boundingRect());
        }

        bool showInsideBorder = sourceProxy->getSourceProperty().isShowInsideBorder();
        if (sourceProxy && showInsideBorder) { // 双边框
            painter->drawRect(rc.adjusted(-5, -5, 5, 5));
        }
    }

    painter->restore();
}

void ImageGraphicsObject::onSourcePropertyChanged(const QString &name, const QVariant &oldValue,
                                                  const QVariant &newValue)
{
    if (imageType == kBackground) {
        if (name == SOURCE_IMAGE_BACKGROUND_PROPERTY_KEY) {
            QByteArray imageData = newValue.toByteArray();
            loadFromData(imageData);
            update();
        } else if (name == SOURCE_BACKGROUND_COLOR_PROPERTY_KEY) {
            fillColor = newValue.value<QColor>();
            update();
        }
        setZValue(-100);
    } else if (imageType == kForeground) {
        if (name == SOURCE_IMAGE_FOREGROUND_PROPERTY_KEY) {
            QByteArray imageData = newValue.toByteArray();
            loadFromData(imageData);
            update();
        }
        setZValue(100);
    }
}
