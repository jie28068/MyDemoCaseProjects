#include "InAndOutSourceGraphicsObject.h"

InAndOutSourceGraphicsObject::InAndOutSourceGraphicsObject(ICanvasScene *canvasScene,
                                                           QSharedPointer<SourceProxy> source, QGraphicsItem *parent)
    : ControlModuleSourceGraphicsObject(canvasScene, source, parent)
{
    // setFlag(ItemIgnoresTransformations, true);
}

InAndOutSourceGraphicsObject::~InAndOutSourceGraphicsObject() { }

void InAndOutSourceGraphicsObject::createInternalGraphics()
{
    ControlModuleSourceGraphicsObject::createInternalGraphics();
}

void InAndOutSourceGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!painter->isActive())
        return;

    QSharedPointer<SourceProxy> sourceProxy = getSourceProxy();
    if (sourceProxy.isNull()) {
        return;
    }
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen pen;
    // pen.setWidthF(1);
    // pen.setCosmetic(true);
    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->setBrush(QBrush("#FFFFFF"));

    QColor backColor = sourceProxy->getSourceProperty().getBackgroundColor();
    if (backColor.isValid()) {
        painter->setBrush(QBrush(backColor));
    }

    TransformProxyGraphicsObject *proxyGraphicsObject = getTransformProxyGraphicsObject();
    if (proxyGraphicsObject) {
        bool isHighLight = proxyGraphicsObject->getLayerProperty().getHightLight();
        if (isHighLight) {
            QColor highLightColor = getCanvasContext()->getCanvasProperty().getHighLightColor();
            pen.setColor(highLightColor);
            painter->setBrush(highLightColor);
        }
    }

    painter->drawRoundedRect(boundingRect(), 10, 10);

    painter->restore();
    SourceGraphicsObject::paint(painter, option, widget);
}

bool InAndOutSourceGraphicsObject::otherPaint(QPainter *painter, const QRectF rect)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen pen;
    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->setBrush(QBrush("#FFFFFF"));

    painter->drawRoundedRect(rect, 10, 10);

    painter->restore();
    return false;
}
