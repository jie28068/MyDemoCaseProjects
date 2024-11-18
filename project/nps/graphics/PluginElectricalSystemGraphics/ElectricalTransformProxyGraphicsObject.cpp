#include "ElectricalTransformProxyGraphicsObject.h"

ElectricalTransformProxyGraphicsObject::ElectricalTransformProxyGraphicsObject(ICanvasScene *canvasScene,
                                                                               QSharedPointer<SourceProxy> sourceProxy,
                                                                               QGraphicsItem *parent)
    : TransformProxyGraphicsObject(canvasScene, sourceProxy)
{
    this->sourceProxy = sourceProxy;
}

QRectF ElectricalTransformProxyGraphicsObject::boundingRect() const
{
    QString prototypeName = sourceProxy->prototypeName();
    if (prototypeName == "Busbar" || prototypeName == "DotBusbar") {
        QString busbarTypeString = sourceProxy->getSourceProperty().getBusbarType();
        if (busbarTypeString == "DotBusbar") {
            return QRect(-5, -5, 10, 10);
        } else {
            return TransformProxyGraphicsObject::boundingRect();
        }

    } else {
        return TransformProxyGraphicsObject::boundingRect();
    }
}
