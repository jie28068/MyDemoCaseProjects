#include "MathMLGraphicsObject.h"
#include "MathMLManager.h"
#include "TransformProxyGraphicsObject.h"
#include "qwt_mml_document.h"

#include <QBuffer>
#include <QByteArray>
#include <QPainter>
#include <QPixmap>
#include <QSvgGenerator>
#include <QSvgRenderer>

class MathMLGrapihcsObjectPrivate
{
public:
    QString mathMLName;
    QRectF boundingRect;
    QSvgRenderer *svgRender;
    QSize renderSize;

    MathMLGrapihcsObjectPrivate()
    {
        mathMLName = "";
        svgRender = nullptr;
    }
};

MathMLGraphicsObject::MathMLGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : GraphicsLayer(canvasScene, parent)
{
    dataPtr.reset(new MathMLGrapihcsObjectPrivate);
}

MathMLGraphicsObject::~MathMLGraphicsObject() { }

void MathMLGraphicsObject::setMathMLName(const QString &name)
{
    if (name != dataPtr->mathMLName) {
        dataPtr->svgRender = MathMLManager::getInstance().getMathMLDocument(name);
        dataPtr->mathMLName = name;
        if (dataPtr->svgRender) {
            dataPtr->renderSize = dataPtr->svgRender->defaultSize();
        }
    }
}

void MathMLGraphicsObject::updatePosition()
{
    auto proxy = getTransformProxyGraphicsObject();
    if (proxy) {
        dataPtr->boundingRect = proxy->boundingRect();
    }
}

void MathMLGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (dataPtr->svgRender) {
        QSizeF svgSize = dataPtr->renderSize;
        QSizeF itemSize = dataPtr->boundingRect.size();
        if (svgSize.width() < itemSize.width() && svgSize.height() < itemSize.height()) {
            QRectF dstRect = QRectF(QPoint(0, 0), svgSize);
            dstRect.moveCenter(boundingRect().center());
            dataPtr->svgRender->render(painter, dstRect);
        } else {
            dataPtr->svgRender->render(painter, boundingRect().adjusted(5, 5, -5, -5));
        }
    }
}

QRectF MathMLGraphicsObject::boundingRect() const
{
    return dataPtr->boundingRect;
}
