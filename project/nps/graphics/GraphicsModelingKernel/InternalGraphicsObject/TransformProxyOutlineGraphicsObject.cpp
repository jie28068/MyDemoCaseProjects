#include "TransformProxyOutlineGraphicsObject.h"
#include "ICanvasScene.h"
#include "NPSPropertyManager.h"
#include "SourceGraphicsObject.h"
#include "TransformProxyGraphicsObject.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

TransformProxyOutlineGraphicsObject::TransformProxyOutlineGraphicsObject(ICanvasScene *canvasScene,
                                                                         QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent)
{
}

TransformProxyOutlineGraphicsObject::~TransformProxyOutlineGraphicsObject() { }

void TransformProxyOutlineGraphicsObject::setShowOutline(bool show)
{
    isShowOutline = show;
}

void TransformProxyOutlineGraphicsObject::updatePosition()
{
    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (!proxyGraphics) {
        return;
    }
    qreal offset = 1;
    proxyRect = proxyGraphics->boundingRect();
}

QRectF TransformProxyOutlineGraphicsObject::boundingRect() const
{
    return proxyRect;
}

void TransformProxyOutlineGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                                QWidget *widget)
{
    auto canvasContext = getCanvasContext();
    if (!canvasContext) {
        return;
    }
    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (!proxyGraphics) {
        return;
    }
    auto sourceProxy = proxyGraphics->getSourceProxy();
    if (!sourceProxy) {
        return;
    }

    bool isHover = false;
    SourceGraphicsObject *sourceGraphics = proxyGraphics->getSourceGraphicsObject();
    if (sourceGraphics) {
        if (sourceGraphics->getLayerFlag() & GraphicsLayer::kCommentGraphicsLayer
            && sourceProxy->prototypeName() != ImageAnnotationStr) { // 图片没有自定义的边框需要显示默认边框
            return;
        }
        isHover = sourceGraphics->isHoverStatus();
    }
    bool isInGroup = (group() != nullptr);
    bool isHighlight = getLayerProperty().getHightLight();
    // bool isGotoFromHighlight = proxyGraphics->getLayerProperty().getGotoFromHightLight();
    if (!(proxyGraphics->isSelected() || isHover || isInGroup || isHighlight)) {
        return;
    }

    painter->save();
    QColor penColor("#00cfff");
    QString stateStr = sourceProxy->state();
    if (stateStr != "normal") {
        penColor = sourceProxy->getStateColor();
    }

    if (isHighlight) {
        penColor = canvasContext->getCanvasProperty().getHighLightColor();
    }

    QPen pen(penColor, 2.0, Qt::SolidLine);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->drawRect(proxyRect);
    painter->restore();
}

void TransformProxyOutlineGraphicsObject::onSourceSizeChanged(QString key, QVariant value)
{
    if (key != GKD::SOURCE_SIZE) {
        return;
    }
    auto proxyGraphics = getTransformProxyGraphicsObject();
    if (!proxyGraphics) {
        return;
    }
    auto sourceGraphics = proxyGraphics->getSourceGraphicsObject();
    if (sourceGraphics) {
        sourceGraphics->setSourceBoundingRect(QRectF(QPointF(0, 0), value.value<QSizeF>()));
        proxyGraphics->setBoundingRect(sourceGraphics->getTransformBoundingRect());
        proxyRect = proxyGraphics->boundingRect();
    }
}