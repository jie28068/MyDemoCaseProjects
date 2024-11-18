#include "TextGraphicsObject.h"
#include "GraphicsKernelDefinition.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"
#include "PortGraphicsObject.h"

#include <QTextDocument>
#include <QFontMetrics>

/*
TextGraphicsObject::TextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent)
{
    textItem = new QGraphicsTextItem(this);
    QTextDocument* document = textItem->document();
    QFont font = QFont("Microsoft YaHei", 6);
    textItem->setFont(font);
}

int TextGraphicsObject::type() const{
    return kTextGraphics;
}

void TextGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                               QWidget *widget)
{
    
}

QRectF TextGraphicsObject::boundingRect() const {
    return textItem->boundingRect();
}

void TextGraphicsObject::updatePosition() {
    QGraphicsItem *parent = parentItem();
    if (parent)
    {
        QRectF proxyRect = parent->boundingRect();
        if (boundingRect().width() > proxyRect.width() ) { 
          textItem->setTextWidth(proxyRect.width());
        } else {
            textItem->setTextWidth(-1);
        }
        QPointF proxyCenter = proxyRect.center();
        setPos(proxyCenter.x() - boundingRect().width() / 2,
               proxyCenter.y() - boundingRect().height() / 2);
    }
}

void TextGraphicsObject::setPlainText(QString text) {
    textItem->setPlainText(text);
}

*/