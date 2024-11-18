#include "ElectricalBusbarGraphicsObject.h"
#include "ElectricalPhaseLineGraphicsObject.h"

#include <QGraphicsItem>
#include <QPainter>

ElectricalBusbarGraphicsObject::ElectricalBusbarGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : ElectricalBaseBusbarObject(canvasScene, parent)
{
    phaseLine = nullptr;
}

ElectricalBusbarGraphicsObject::~ElectricalBusbarGraphicsObject() { }

void ElectricalBusbarGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    SourceGraphicsObject *sourceGraphics = dynamic_cast<SourceGraphicsObject *>(parentItem());
    if (sourceGraphics && sourceGraphics->getSourceProxy()) {
        painter->setRenderHint(QPainter::Antialiasing, false);
        QPen pen(Qt::SolidLine);

        GraphicsLayerProperty &layerPorperty = getTransformProxyGraphicsObject()->getLayerProperty();
        if (layerPorperty.getHightLight()) {
            auto canvasContext = getCanvasContext();
            if (canvasContext) {
                QColor highLightColor = canvasContext->getCanvasProperty().getHighLightColor();
                painter->setBrush(highLightColor);
                pen.setColor(highLightColor);
            } else {
                painter->setBrush(QColor("#0079c2"));
                pen.setColor(QColor("#0079c2"));
            }

        } else {
            painter->setBrush(sourceGraphics->getSourceProxy()->getStateColor());
            pen.setColor(sourceGraphics->getSourceProxy()->getStateColor());
        }
        painter->setPen(pen);
        if(m_bShowPowerFlow){
            painter->setBrush(m_showColor);
        }
        painter->drawRect(boundingRect().adjusted(0, 3.5, 0, -3.5));
    }
    painter->restore();
}

QVariant ElectricalBusbarGraphicsObject::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case QGraphicsItem::ItemVisibleHasChanged: {
        bool flag = isVisible();
        if (phaseLine) {
            phaseLine->userShow(flag);
        }
        
    }
    default: {
    }
    }
    return GraphicsLayer::itemChange(change, value);
}

QRectF ElectricalBusbarGraphicsObject::boundingRect() const
{
    SourceGraphicsObject *sourceGraphics = dynamic_cast<SourceGraphicsObject *>(parentItem());
    if (sourceGraphics) {
        return sourceGraphics->boundingRect();
    } else {
        return QRectF(0, 0, 0, 0);
    }
}

void ElectricalBusbarGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event) 
{ 
    GraphicsLayer::mousePressEvent(event);
}

void ElectricalBusbarGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event) 
{ 
    GraphicsLayer::mouseMoveEvent(event);
}

void ElectricalBusbarGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) 
{ 
    GraphicsLayer::mouseReleaseEvent(event);
}

void ElectricalBusbarGraphicsObject::createInternalGraphics()
{
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    phaseLine = new ElectricalPhaseLineGraphicsObject(getCanvasScene(), proxyGraphics);
    phaseLine->createInternalGraphics();
    phaseLine->updatePosition();
}

/* void ElectricalBusbarGraphicsObject::updatePosition() 
{ 
    if(virtualPort)
    {
        updateVirtualPortPos(mapToScene(virtualPort->pos()));
    }
    GraphicsLayer::updatePosition();
} */

/* void ElectricalBusbarGraphicsObject::updateVirtualPortPos(QPointF pos) 
{ 
    if(virtualPort)
    {
        TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
        if(proxyGraphics)
        {
            if(proxyGraphics->getAngle() % 180 == 0)
            {
                virtualPort->setPos(proxyGraphics->mapFromScene(pos).x(), proxyGraphics->boundingRect().bottom() + 10);
            }
            else
            {
                virtualPort->setPos(proxyGraphics->boundingRect().right() + 10, proxyGraphics->mapFromScene(pos).y());
            }
        }
    }
}

void ElectricalBusbarGraphicsObject::onSelectedChanged(bool selected)
{
    if(virtualPort)
    {
        virtualPort->userShow(selected);
    }
} */
