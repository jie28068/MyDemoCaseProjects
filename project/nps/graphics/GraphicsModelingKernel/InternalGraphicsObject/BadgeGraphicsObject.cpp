#include "BadgeGraphicsObject.h"
#include "CanvasContext.h"
#include "TransformProxyGraphicsObject.h"

#include <QPainter>

BadgeGraphicsObject::BadgeGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent)
{
    auto canvasContext = getCanvasContext();
    if (canvasContext) {
        connect(canvasContext.data(), &CanvasContext::showModelRunningSortChanged, this,
                &BadgeGraphicsObject::onShowModelRunningSortChanged);
    }
    font.setPixelSize(10);
    font.setFamily("Microsoft YaHei UI");
}

BadgeGraphicsObject::~BadgeGraphicsObject() { }

void BadgeGraphicsObject::updatePosition()
{
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    if (proxyGraphics) {
        QRectF proxyRect = proxyGraphics->boundingRect();
        QPointF topLeft = proxyRect.topLeft();
        QPointF pt = topLeft - QPointF(20, 20);
        setPos(pt);
    }
}

void BadgeGraphicsObject::setMsgTip(QString msg)
{
    msgTip = msg;
    if (msgTip.isEmpty()) {
        userShow(false);
    } else {
        auto canvasContext = getCanvasContext();
        if (canvasContext) {
            bool enableShow = canvasContext->isShowModelRunningSort();
            userShow(enableShow);
        }
    }

    if (msgTip.length() >= 3) {
        int fontSize = 10;
        while (fontSize >= 4) {
            font.setPixelSize(fontSize--);
            QFontMetrics fontMetrics(font);
            int width = fontMetrics.width(msgTip);
            if (width <= boundingRect().width()) {
                break;
            }
        }
    }
}

void BadgeGraphicsObject::userShow(bool visible)
{
    if (!visible) {
        setVisible(false);
    } else {
        if (msgTip.isEmpty()) {
            setVisible(false);
        } else {
            setVisible(true);
        }
    }
}

void BadgeGraphicsObject::onShowModelRunningSortChanged(bool show)
{
    userShow(show);
}

QRectF BadgeGraphicsObject::boundingRect() const
{
    return QRectF(0, 0, 20, 20);
}

void BadgeGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    painter->setBrush(Qt::green);
    painter->drawEllipse(boundingRect());

    QPen pen = QPen(Qt::black);
    painter->setPen(pen);
    painter->setFont(font);

    painter->drawText(boundingRect(), Qt::AlignCenter, msgTip);

    painter->restore();
}
