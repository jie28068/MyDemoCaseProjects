#include "GainMouduleSourceGraphicsObject.h"

GainMouduleSourceGraphicsObject::GainMouduleSourceGraphicsObject(ICanvasScene *canvasScene,
                                                                 QSharedPointer<SourceProxy> source,
                                                                 QGraphicsItem *parent)
    : ControlModuleSourceGraphicsObject(canvasScene, source, parent)
{
    // setFlag(ItemIgnoresTransformations, true);
}

GainMouduleSourceGraphicsObject::~GainMouduleSourceGraphicsObject() { }

void GainMouduleSourceGraphicsObject::createInternalGraphics()
{
    ControlModuleSourceGraphicsObject::createInternalGraphics();
    setScaleDirection(SourceGraphicsObject::kScaleAll);
}

void GainMouduleSourceGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!painter->isActive())
        return;

    QSharedPointer<SourceProxy> sourceProxy = getSourceProxy();
    if (sourceProxy.isNull()) {
        return;
    }
    painter->save();
    QPen pen;
    // pen.setColor(sourceProxy->getStateColor());
    pen.setCosmetic(true);
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

    painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

    QPolygonF points;
    points.push_back(boundingRect().topLeft());
    points.push_back(boundingRect().bottomLeft());
    points.push_back(QPointF(boundingRect().right(), boundingRect().center().y()));
    QPainterPath path;
    path.addPolygon(points);
    path.closeSubpath();
    painter->drawPath(path);

    painter->restore();

    SourceGraphicsObject::paint(painter, option, widget);
}
