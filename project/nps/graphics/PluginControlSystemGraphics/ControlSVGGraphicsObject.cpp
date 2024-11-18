#include "ControlSVGGraphicsObject.h"
#include "defines.h"

#include <QSvgRenderer>

ControlSVGGraphicsObject::ControlSVGGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : SvgGraphicsObject(canvasScene, parent)
{
    proxyGraphicsObject = getTransformProxyGraphicsObject();
}

ControlSVGGraphicsObject::~ControlSVGGraphicsObject() { }

QRectF ControlSVGGraphicsObject::boundingRect() const
{
    if (proxyGraphicsObject) {
        return proxyGraphicsObject->boundingRect();
    }
    return getProxyRect();
}

void ControlSVGGraphicsObject::updatePosition() { }

void ControlSVGGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto svgRender = getSvgRenderer();

    QRectF rect = boundingRect(); //
    if (svgRender) {
        svgRender->setAspectRatioMode(Qt::KeepAspectRatio);
        QSizeF svgSize = svgRender->defaultSize();
        QSizeF itemSize = rect.size();
        if (svgSize.width() < itemSize.width() && svgSize.height() < itemSize.height()) {
            QRectF dstRect = QRectF(QPoint(0, 0), svgSize);
            dstRect.moveCenter(rect.center());
            svgRender->render(painter, dstRect);
        } else {
            if (svgSize.width() > 100 || svgSize.height() > 100) {
                rect = rect.adjusted(10, 10, -10, -10);
            }
            svgRender->render(painter, rect);
        }
    }
}
