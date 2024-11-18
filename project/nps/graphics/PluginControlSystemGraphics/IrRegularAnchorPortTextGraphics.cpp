#include "IrRegularAnchorPortTextGraphics.h"
#include "IrRegularAnchorPortGraphics.h"

#include <QFontMetrics>
#include <QTextDocument>

IrRegularPortNameTextGraphicsObject::IrRegularPortNameTextGraphicsObject(ICanvasScene *canvasScene,
                                                                         QGraphicsItem *parent)
    : TextGraphicsObject(canvasScene, parent)
{
    setType(kPortNameTextGraphics);
    textItem->setAcceptHoverEvents(false);
}

void IrRegularPortNameTextGraphicsObject::updatePosition()
{
    IrRegularAnchorPortGraphics *portGraphics = dynamic_cast<IrRegularAnchorPortGraphics *>(parentItem());
    if (!portGraphics) {
        return GraphicsLayer::updatePosition();
    }
    auto *sourceGraphics = portGraphics->getSourceGraphics();
    if (!sourceGraphics) {
        return GraphicsLayer::updatePosition();
    }
    QPointF portLinkPos = portGraphics->getPortNamePos();
    QRectF rc = boundingRect();
    QPointF pt = portGraphics->mapFromScene(portLinkPos);
    setPos(pt - rc.center());

    GraphicsLayer::updatePosition();
}