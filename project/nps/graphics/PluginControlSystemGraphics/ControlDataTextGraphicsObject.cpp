#include "ControlDataTextGraphicsObject.h"

ControlDataTextGraphicsObject::ControlDataTextGraphicsObject(ICanvasScene *canvasScene, QGraphicsItem *parent)
    : TextGraphicsObject<QGraphicsTextItem>(canvasScene, parent)
{
    // textItem->setAcceptHoverEvents(false);
    int layerFlag = kTextGraphicsLayer | kMoveableGraphicsLayer | kSelectedGraphicsLayer;
    setLayerFlag((GraphicsLayer::GraphicsLayerFlag)layerFlag);
    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
    setType(kDataTextGraphics);
    QFont font = textItem->font();
    font.setPixelSize(10);
    textItem->setFont(font);
    // setLayerFlag(GraphicsLayer::kCommentGraphicsLayer);
}

ControlDataTextGraphicsObject::~ControlDataTextGraphicsObject() { }

void ControlDataTextGraphicsObject::updatePosition()
{
    if (!isVisible()) {
        return;
    }
    PortGraphicsObject *portGraphics = dynamic_cast<PortGraphicsObject *>(parentItem());
    if (portGraphics) {
        QRectF rect = portGraphics->boundingRect();
        QPointF centerPos = rect.center();
        QRectF boundRect = boundingRect();
        Qt::AnchorPoint anchor = portGraphics->getAnchorPoint();
        QPointF defaultPos;
        switch (anchor) {
        case Qt::AnchorTop:
            defaultPos = QPointF(centerPos.x() - boundRect.width() / 2, rect.top() - 5 - boundRect.height());
            break;
        case Qt::AnchorLeft:
            defaultPos = QPointF(rect.left() - boundRect.width(), centerPos.y() - boundRect.height() / 2);
            break;
        case Qt::AnchorBottom:
            defaultPos = QPointF(centerPos.x() - boundingRect().width() / 2, rect.bottom() + 5);
            break;
        case Qt::AnchorRight:
            defaultPos = QPointF(rect.right() + 5, centerPos.y() - boundRect.height() / 2);
            break;
        }
        setPos(defaultPos);
    }
    // GraphicsLayer::updatePosition();
}

QRectF ControlDataTextGraphicsObject::boundingRect() const
{
    QRectF rect(0, 0, 0, 0);
    QRectF rcText = textItem->boundingRect();
    if (rcText.width() > rect.width()) {
        rect.setWidth(rcText.width());
    }
    if (rcText.height() > rect.height()) {
        rect.setHeight(rcText.height());
    }
    int adjustSize = 2;
    return rect.adjusted(-adjustSize, -adjustSize, adjustSize, adjustSize);
}

void ControlDataTextGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    GraphicsLayer::paint(painter, option, widget);
    // painter->save();
    // QPen pen;
    // painter->setBrush(QBrush("white"));
    // painter->setPen(Qt::NoPen);
    // // pen.setWidthF(0.2);
    // painter->drawRect(boundingRect());
    // painter->restore();
}