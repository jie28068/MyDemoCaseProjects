#include "SourceProtoTypeTextGraphicsObject.h"
#include "TransformProxyGraphicsObject.h"

#include <QTextBlock>

SourceProtoTypeTextGraphicsObject::SourceProtoTypeTextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : TextGraphicsObject<QGraphicsTextItem>(canvasScene, parent), isSubSystemPrototype(false)
{
    textItem->setAcceptHoverEvents(false);
    QFont font = this->font();
    font.setPixelSize(10);
    font.setFamily("Microsoft YaHei UI");
    this->setfont(font);
    document()->setDocumentMargin(0);
    auto sourceProxy = getSourceProxy();
    if (sourceProxy) {
        QString prototypeName = sourceProxy->prototypeName();
        if (prototypeName.startsWith("*Subsystem@")) {
            // 子系统不显示原型名称
            isSubSystemPrototype = true;
            setVisible(false);
        }
    }
}

SourceProtoTypeTextGraphicsObject::~SourceProtoTypeTextGraphicsObject() { }

void SourceProtoTypeTextGraphicsObject::updatePosition()
{
    QGraphicsItem *parent = parentItem();
    if (parent) {
        QRectF proxyRect = parent->boundingRect();
        // if (boundingRect().width() >= proxyRect.width()) {
        //     textItem->setTextWidth(proxyRect.width());
        // } else {
        //     textItem->setTextWidth(-1);
        // }

        QString prototypeName = textItem->toPlainText();

        QFontMetrics fontMetrics(textItem->font());
        QRectF fontRect = fontMetrics.boundingRect(prototypeName);
        if (fontRect.width() > proxyRect.width()) {
            textItem->setTextWidth(proxyRect.width());
        } else {
            textItem->setTextWidth(-1);
        }

        QPointF proxyCenter = proxyRect.center();
        // setPos(proxyCenter);
        setPos(proxyCenter.x() - boundingRect().width() / 2, proxyCenter.y() - boundingRect().height() / 2);
    }
}

void SourceProtoTypeTextGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                              QWidget *widget)
{
    TextGraphicsObject<QGraphicsTextItem>::paint(painter, option, widget);

    TransformProxyGraphicsObject *tranObj = getTransformProxyGraphicsObject();
    if (!tranObj)
        return;
    QSharedPointer<SourceProxy> sourceProxy = tranObj->getSourceProxy();
    if (!sourceProxy)
        return;

    if (textItem->defaultTextColor() != sourceProxy->getStateColor()) {
        textItem->setDefaultTextColor(sourceProxy->getStateColor());
        textItem->update();
    }
}

QRectF SourceProtoTypeTextGraphicsObject::boundingRect() const
{

    QRectF rcText = textItem->boundingRect();
    return rcText;

    return QRectF(-rcText.width() / 2, -rcText.height() / 2, rcText.width(), rcText.height());
}

void SourceProtoTypeTextGraphicsObject::userShow(bool visible)
{
    if (isSubSystemPrototype) {
        setVisible(false);
    } else {
        GraphicsLayer::userShow(visible);
    }
}
