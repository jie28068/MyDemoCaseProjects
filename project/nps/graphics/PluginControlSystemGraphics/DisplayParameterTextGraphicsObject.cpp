#include "DisplayParameterTextGraphicsObject.h"
#include <QDebug>

DisplayParameterTextGraphicsObject::DisplayParameterTextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : TextGraphicsObject<QGraphicsTextItem>(canvasScene, parent)
{
    textItem->setAcceptHoverEvents(false);
    QFont font = this->font();
    font.setPixelSize(10);
    font.setFamily("Microsoft YaHei UI");
    this->setfont(font);
    document()->setDocumentMargin(0);
}

DisplayParameterTextGraphicsObject::~DisplayParameterTextGraphicsObject() { }

void DisplayParameterTextGraphicsObject::updatePosition()
{
    QGraphicsItem *parent = parentItem();
    if (parent) {
        QRectF proxyRect = parent->boundingRect();
        /* if (boundingRect().width() >= proxyRect.width()) {
            textItem->setTextWidth(proxyRect.width());
        } else {
            textItem->setTextWidth(-1);
        } */
        textItem->setTextWidth(-1);
        QString displayText = getSourceProxy()->getSourceProperty().getDisplayData().toString();
        QFontMetrics fontMetrics(textItem->font());
        if (fontMetrics.width(displayText) > parent->boundingRect().width()) {
            displayText = fontMetrics.elidedText(displayText, Qt::ElideRight, parent->boundingRect().width());
        }
        QRectF fontRect = fontMetrics.boundingRect(displayText);
        setPlainText(displayText);
        QPointF proxyCenter = proxyRect.center();
        setPos(proxyCenter.x() - fontRect.width() / 2, proxyCenter.y() - fontRect.height() / 2);
    }
}

void DisplayParameterTextGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
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
