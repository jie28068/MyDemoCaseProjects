#include "GoToFromSourceGraphicsObjct.h"

GoToFromSourceGraphicsObjct::GoToFromSourceGraphicsObjct(ICanvasScene *canvasScene, QSharedPointer<SourceProxy> source,
                                                         QGraphicsItem *parent)
    : ControlModuleSourceGraphicsObject(canvasScene, source, parent)
{
    setSourceBoundingRect();
}

GoToFromSourceGraphicsObjct::~GoToFromSourceGraphicsObjct() { }

void GoToFromSourceGraphicsObjct::createInternalGraphics()
{
    ControlModuleSourceGraphicsObject::createInternalGraphics();
    // PSourceProxy sourceProxy = getSourceProxy();
    // if (!sourceProxy) {
    //     return;
    // }
    // SourceProperty &sourceProperty = sourceProxy->getSourceProperty();

    // TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    // // 如果有需要展示的参数 就创建displayText图元
    // displayText = new DisplayParameterTextGraphicsObject(getCanvasScene(), proxyGraphics);
    // QString text = sourceProperty.getDisplayData().toString();
    // if (text.isEmpty() && sourceProxy->prototypeName() == "Goto") {
    //     text = sourceProxy->name();
    //     sourceProperty.setDisplayData(text);
    // }
    // displayText->setPlainText(text);
    // displayText->setLayerFlag(GraphicsLayer::kSourceGraphicsLayer);
    // displayText->updatePosition();

    // bool allowScale = sourceProxy->getSourceProperty().isEnableScale();
    // if (allowScale) {
    //     setScaleDirection(SourceGraphicsObject::kScaleAll);
    // } else {
    //     setScaleDirection(SourceGraphicsObject::kScaleNone);
    // }
}

void GoToFromSourceGraphicsObjct::setSourceBoundingRect(QRectF rect)
{
    ControlModuleSourceGraphicsObject::setSourceBoundingRect(rect);
    QRectF rc = boundingRect();
    shapePoints.clear();
    if (getSourceProxy()->prototypeName() == "Goto") {
        QPointF topCenterLeft(rc.topLeft().x() + rc.width() * 0.25, rc.topLeft().y());
        shapePoints.push_back(topCenterLeft);
        shapePoints.push_back(rc.topRight());
        shapePoints.push_back(rc.bottomRight());
        QPointF bottomCenterLeft(rc.bottomLeft().x() + rc.width() * 0.25, rc.bottomLeft().y());
        shapePoints.push_back(bottomCenterLeft);
        QPointF centerLeft(rc.left(), rc.center().y());
        shapePoints.push_back(centerLeft);
    } else {
        QPointF topCenterRight(rc.topRight().x() - rc.width() * 0.25, rc.topRight().y());
        shapePoints.push_back(topCenterRight);
        shapePoints.push_back(rc.topLeft());
        shapePoints.push_back(rc.bottomLeft());
        QPointF bottomCenterRight(rc.bottomRight().x() - rc.width() * 0.25, rc.bottomRight().y());
        shapePoints.push_back(bottomCenterRight);
        QPointF centerRight(rc.right(), rc.center().y());
        shapePoints.push_back(centerRight);
    }
}

void GoToFromSourceGraphicsObjct::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!painter->isActive())
        return;

    QSharedPointer<SourceProxy> sourceProxy = getSourceProxy();
    if (sourceProxy.isNull()) {
        return;
    }
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, false);
    QPen pen;
    pen.setWidthF(1);
    pen.setColor(sourceProxy->getStateColor());
    pen.setCosmetic(true);
    pen.setColor(Qt::black);
    QColor brushColor(255, 255, 255);
    QColor backColor = sourceProxy->getSourceProperty().getBackgroundColor();
    if (backColor.isValid()) {
        brushColor = backColor;
    }
    TransformProxyGraphicsObject *proxyGraphicsObject = getTransformProxyGraphicsObject();
    if (proxyGraphicsObject) {
        bool isHighLight = proxyGraphicsObject->getLayerProperty().getHightLight();
        if (isHighLight) {
            brushColor = getCanvasContext()->getCanvasProperty().getHighLightColor();
        }
        bool isGotoFromHighlight = proxyGraphicsObject->getLayerProperty().getGotoFromHightLight();
        if (isGotoFromHighlight) {
            brushColor = getCanvasContext()->getCanvasProperty().getHighLightColor();
        }
    }
    painter->setBrush(QBrush(brushColor));
    if (shapePoints.size() >= 5) {
        QPainterPath path;
        path.addPolygon(shapePoints);
        path.closeSubpath();
        painter->setPen(Qt::NoPen);
        // painter->fillPath(path, QBrush(brushColor));
        painter->setPen(pen);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawPath(path);
    }
    painter->restore();

    SourceGraphicsObject::paint(painter, option, widget);
}

bool GoToFromSourceGraphicsObjct::otherPaint(QPainter *painter, const QRectF rect)
{
    return false;
}
