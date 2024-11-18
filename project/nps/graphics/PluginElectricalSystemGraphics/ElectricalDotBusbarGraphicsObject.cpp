#include "ElectricalDotBusbarGraphicsObject.h"

#include <QPainter>

ElectricalDotBusbarGraphicsObject::ElectricalDotBusbarGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : ElectricalBaseBusbarObject(canvasScene, parent)
{
}

ElectricalDotBusbarGraphicsObject::~ElectricalDotBusbarGraphicsObject() { }

void ElectricalDotBusbarGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                              QWidget *widget)
{
    SourceGraphicsObject *sourceGraphics = dynamic_cast<SourceGraphicsObject *>(parentItem());
    if (sourceGraphics && sourceGraphics->getSourceProxy()) {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(sourceGraphics->getSourceProxy()->getStateColor());
        auto transformGraphics = getTransformProxyGraphicsObject();
        if (transformGraphics) {
            GraphicsLayerProperty &layerPorperty = transformGraphics->getLayerProperty();
            if (layerPorperty.getHightLight()) {
                auto canvasContext = getCanvasContext();
                if (canvasContext) {
                    QColor highLightColor = canvasContext->getCanvasProperty().getHighLightColor();
                    painter->setBrush(highLightColor);
                } else {
                    painter->setBrush(QColor("#0079c2"));
                }
            }
        }
        int size = 5;
        if(m_bShowPowerFlow){
            painter->setBrush(m_showColor);
        }
        painter->drawEllipse(QRectF(size / 2.f, size / 2.f, size, size));
        painter->restore();
    }
}

QRectF ElectricalDotBusbarGraphicsObject::boundingRect() const
{
    SourceGraphicsObject *sourceGraphics = dynamic_cast<SourceGraphicsObject *>(parentItem());
    if (sourceGraphics) {
        return sourceGraphics->boundingRect();
    } else {
        return QRectF(0, 0, 0, 0);
    }
}