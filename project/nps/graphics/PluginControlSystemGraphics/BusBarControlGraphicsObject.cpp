#include "BusBarControlGraphicsObject.h"
#include "GraphicsModelingKernel/GraphicsLayer.h"

BusSelectorImage::BusSelectorImage(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent)
{
    proxyGraphics = getTransformProxyGraphicsObject();
    setLayerFlag(GraphicsLayer::kBackgroundLayer);
}

void BusSelectorImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!painter->isActive())
        return;

    QSharedPointer<SourceProxy> sourceProxy = getSourceProxy();
    if (sourceProxy.isNull()) {
        return;
    }
    QString type = sourceProxy->prototypeName();
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, false);
    QRectF rect = boundingRect();
    QPen pen;
    pen.setWidthF(1);
    pen.setColor(sourceProxy->getStateColor());
    pen.setCosmetic(true);
    painter->setBrush(QBrush("#000000"));
    painter->drawRect(rect);
    // painter->setPen(pen);
    if (type == "BusCreator" || type == "BusSelector") {
        painter->setBrush(QBrush("#FFFFFF"));
        QPointF centerPos = rect.center();
        painter->drawRect(QRectF(rect.left(), centerPos.y() - 8, rect.width(), 15));
        pen.setColor("#000000");
        painter->setPen(pen);
        painter->drawLine(rect.left(), centerPos.y() - 3, rect.right(), centerPos.y() - 3);
        painter->drawLine(rect.left(), centerPos.y() + 2, rect.right(), centerPos.y() + 2);
    }
    painter->restore();
}

QRectF BusSelectorImage::boundingRect() const
{
    if (proxyGraphics) {
        return proxyGraphics->boundingRect();
    }
    return QRectF();
}

BusBarControlGraphicsObject::BusBarControlGraphicsObject(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                                         QGraphicsItem *parent)
    : ControlModuleSourceGraphicsObject(canvasScene, source, parent)
{
    // setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

BusBarControlGraphicsObject::~BusBarControlGraphicsObject() { }

void BusBarControlGraphicsObject::createInternalGraphics()
{
    imageItem = new BusSelectorImage(getCanvasScene(), getTransformProxyGraphicsObject());
    PSourceProxy sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    SourceProperty &sourceProperty = sourceProxy->getSourceProperty();
    bool allowScale = sourceProperty.isEnableScale();
    if (allowScale) {
        setScaleDirection(SourceGraphicsObject::kScaleAll);
    } else {
        setScaleDirection(SourceGraphicsObject::kScaleNone);
    }
}

void BusBarControlGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // if (!painter->isActive())
    //     return;

    // QSharedPointer<SourceProxy> sourceProxy = getSourceProxy();
    // if (sourceProxy.isNull()) {
    //     return;
    // }
    // QString type = sourceProxy->prototypeName();
    // painter->save();
    // painter->setRenderHint(QPainter::Antialiasing, false);
    // QRectF rect = boundingRect();
    // QPen pen;
    // pen.setWidthF(1);
    // pen.setColor(sourceProxy->getStateColor());
    // pen.setCosmetic(true);
    // painter->setBrush(QBrush("#000000"));
    // painter->drawRect(rect);
    // // painter->setPen(pen);
    // if (type == "BusCreator" || type == "BusSelector") {
    //     painter->setBrush(QBrush("#FFFFFF"));
    //     QPointF centerPos = rect.center();
    //     painter->drawRect(QRectF(rect.left(), centerPos.y() - 7.5, rect.width(), 15));
    //     pen.setColor("#000000");
    //     painter->setPen(pen);
    //     painter->drawLine(rect.left(), centerPos.y() - 2, rect.right(), centerPos.y() - 2);
    //     painter->drawLine(rect.left(), centerPos.y() + 2, rect.right(), centerPos.y() + 2);
    // }
    // painter->restore();

    SourceGraphicsObject::paint(painter, option, widget);
}
