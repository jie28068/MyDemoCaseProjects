#include "ElectPortNameGraphics.h"
#include <QFontMetrics>
#include <QTextDocument>


ElectPortNameGraphics::ElectPortNameGraphics(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : PortNameTextGraphicsObject(canvasScene, parent)
{
    textFont.setPointSizeF(5);
    textFont.setFamily("Microsoft YaHei UI");
    textItem->setFont(textFont);
}

void ElectPortNameGraphics::updatePosition()
{
    PortGraphicsObject *portGraphics = dynamic_cast<PortGraphicsObject *>(parentItem());
    if (!portGraphics) {
        return GraphicsLayer::updatePosition();
    }
    auto *sourceGraphics = portGraphics->getSourceGraphics();
    if (!sourceGraphics) {
        return GraphicsLayer::updatePosition();
    }

    setTransform(QTransform());
    QRectF textRect = boundingRect();
    QRectF portRect = portGraphics->boundingRect();
    QPointF portCenter = portRect.center();
    setPos(portCenter.x(), portCenter.y() - boundingRect().height() / 2);
    QPointF nowPos = pos();
    Qt::AnchorPoint anchor = portGraphics->getAnchorPoint();
    PSourceProxy sourceProxy = getTransformProxyGraphicsObject()->getSourceProxy();
    double shift = 2.0;

    switch (anchor) {
    case Qt::AnchorTop:
        nowPos.setX(portCenter.x()  - textRect.width() / 2 );
        nowPos.setY(portRect.top() - textRect.height());
        setPos(nowPos);
        break;
    case Qt::AnchorRight:
        nowPos.setX(portRect.right() + shift);
        nowPos.setY(portCenter.y() - textRect.height() / 2);
        setPos(nowPos);
        break;
    case Qt::AnchorBottom:
        nowPos.setX(portCenter.x() - textRect.width() / 2);
        nowPos.setY(portRect.bottom() + shift);
        setPos(nowPos);
        break;
    case Qt::AnchorLeft:
        nowPos.setX(portRect.left() - textRect.width() - shift);
        nowPos.setY(portCenter.y() - textRect.height() / 2);
        setPos(nowPos);
        break;
    default: {
    }
    }

    GraphicsLayer::updatePosition();
}

QRectF ElectPortNameGraphics::boundingRect()
{
   return textItem->boundingRect();
    // return rcText;
    /* int adjustSize = 2;
    return rcText.adjusted(-adjustSize, -adjustSize, adjustSize, adjustSize); */
}
