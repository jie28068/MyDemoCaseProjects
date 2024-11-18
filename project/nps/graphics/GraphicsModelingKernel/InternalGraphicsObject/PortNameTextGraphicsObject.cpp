#include "PortNameTextGraphicsObject.h"

#include "PortGraphicsObject.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"
#include <QFontMetrics>
#include <QTextDocument>


PortNameTextGraphicsObject::PortNameTextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : TextGraphicsObject(canvasScene, parent)
{
    setType(kPortNameTextGraphics);
    textItem->setAcceptHoverEvents(false);
    textFont.setPointSizeF(8);
    textFont.setFamily("Microsoft YaHei UI");
    textItem->setFont(textFont);
}

void PortNameTextGraphicsObject::updatePosition()
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
    QFont font = textItem->font();
    QFontMetrics fm(font);
    int fontHeight = fm.height();
    QPointF portCenter = portRect.center();
    setPos(portCenter.x(), portCenter.y() - boundingRect().height() / 2);
    QPointF nowPos = pos();
    Qt::AnchorPoint anchor = portGraphics->getAnchorPoint();
    PSourceProxy sourceProxy = getTransformProxyGraphicsObject()->getSourceProxy();
    // 文字距离边框距离
    int offset = 6;
    // 如果资源绘制内边框，那么需要调整文字和边框的间距
    bool showInsideBorder = sourceProxy->getSourceProperty().isShowInsideBorder();
    if (sourceProxy && showInsideBorder) {
        offset = 8;
    }
    // switch 和multiportSwitch需要设置一个偏移
    double shift = 0.0;
    if(sourceProxy->prototypeName() == "Switch" || sourceProxy->prototypeName() == "MultiportSwitch"){
        shift = 5.0;
    }

    switch (anchor) {
    case Qt::AnchorTop:
        nowPos.setX(nowPos.x() - textRect.width() / 2 + 2 + shift);
        nowPos.setY(nowPos.y() + textRect.width() / 2 + offset);
        setPos(nowPos);
        Utility::rotateOnCenter(this, 90);
        textRect = boundingRect();
        break;
    case Qt::AnchorRight:
        nowPos.setX(nowPos.x() - textRect.width() / 2 - boundingRect().width() / 2 - offset);
        nowPos.setY(nowPos.y() + 2 - shift);
        setPos(nowPos);
        break;
    case Qt::AnchorBottom:
        nowPos.setX(nowPos.x() - textRect.width() / 2 + 2 + shift );
        nowPos.setY(nowPos.y() - textRect.width() / 2 - offset);
        setPos(nowPos);
        Utility::rotateOnCenter(this, -90);
        break;
    case Qt::AnchorLeft:
        nowPos.setX(nowPos.x() + portRect.width() / 2 + offset);
        nowPos.setY(nowPos.y() + 2 - shift);
        setPos(nowPos);
        break;
    default: {
    }
    }

    GraphicsLayer::updatePosition();
}

void PortNameTextGraphicsObject::userShow(bool visible)
{
    GraphicsLayer::userShow(visible);
}
