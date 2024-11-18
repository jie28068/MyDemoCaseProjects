#include "CircleSumSourceGraphicsObject.h"

CircleSumSourceGraphicsObject::CircleSumSourceGraphicsObject(ICanvasScene *canvasScene,
                                                             QSharedPointer<SourceProxy> source, QGraphicsItem *parent)
    : ControlModuleSourceGraphicsObject(canvasScene, source, parent)
{
    // setFlag(ItemIgnoresTransformations, true);
    connect(source.data(), &SourceProxy::sigResetPortList, this, &CircleSumSourceGraphicsObject::onPortReset);
}

CircleSumSourceGraphicsObject::~CircleSumSourceGraphicsObject() { }

void CircleSumSourceGraphicsObject::onPortReset(int size)
{
    auto sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }
    auto portList = sourceProxy->portList();
    int inputPortCounts = 0 > portList.size() - 1 ? 0 : portList.size() - 1;
    int index = 1;
    switch (inputPortCounts) {
    case 1: {
        foreach (auto port, portList) {
            if (port->type() == "input") {
                port->setAngle(180);
            }
        }
    } break;
    case 2: {
        foreach (auto port, portList) {
            if (port->type() == "input") {
                if (index == 1) {
                    port->setAngle(270);
                    index++;
                } else {
                    port->setAngle(90);
                }
            }
        }
    } break;
    case 3: {
        foreach (auto port, portList) {
            if (port->type() == "input") {
                if (index == 1) {
                    port->setAngle(270);
                } else if (index == 2) {
                    port->setAngle(180);
                } else if (index == 3) {
                    port->setAngle(90);
                }
                index++;
            }
        }
    } break;
    case 4: {
        foreach (auto port, portList) {
            if (port->type() == "input") {
                port->setAngle(45 + (4 - index) * 90);
                index++;
            }
        }

    } break;
    default: {
        int portAngle = floor(double(360 / (inputPortCounts + 1)));
        foreach (auto port, portList) {
            if (port->type() == "input") {
                port->setAngle(45 + portAngle * (inputPortCounts - index));
                index++;
            }
        }
    }
    }
}

void CircleSumSourceGraphicsObject::createInternalGraphics()
{
    PSourceProxy sourceProxy = getSourceProxy();
    if (!sourceProxy) {
        return;
    }

    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    if (!sourceProxy->svgImageDatas().isEmpty()) {
        controlSvgGraphics = new ControlSVGGraphicsObject(getCanvasScene(), proxyGraphics);
        controlSvgGraphics->setSvgImageDatas(sourceProxy->svgImageDatas());
        controlSvgGraphics->setLayerFlag(GraphicsLayer::kSourceGraphicsLayer);
        controlSvgGraphics->updatePosition();
    } else {
        protoTypeText = new SourceProtoTypeTextGraphicsObject(getCanvasScene(), proxyGraphics);
        protoTypeText->setPlainText("∑");
        QFont font = protoTypeText->font();
        font.setPointSizeF(20);
        font.setFamily("Microsoft YaHei UI");
        protoTypeText->setfont(font);
        protoTypeText->setLayerFlag(GraphicsLayer::kSourceGraphicsLayer);
        protoTypeText->updatePosition();
    }
    onPortReset(0);

    setScaleDirection(SourceGraphicsObject::kScaleNone);
}

void CircleSumSourceGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!painter->isActive())
        return;

    QSharedPointer<SourceProxy> sourceProxy = getSourceProxy();
    if (sourceProxy.isNull()) {
        return;
    }
    painter->save();

    QPen pen;
    pen.setColor(sourceProxy->getStateColor());

    painter->setPen(pen);
    painter->setBrush(QBrush("#FFFFFF"));

    QColor backColor = sourceProxy->getSourceProperty().getBackgroundColor();
    if (backColor.isValid()) {
        painter->setBrush(QBrush(backColor));
    }

    TransformProxyGraphicsObject *proxyGraphicsObject = getTransformProxyGraphicsObject();
    if (proxyGraphicsObject) {
        bool isHighLight = proxyGraphicsObject->getLayerProperty().getHightLight();
        if (isHighLight) {
            QColor highLightColor = getCanvasContext()->getCanvasProperty().getHighLightColor();
            pen.setColor(highLightColor);
            painter->setBrush(highLightColor);
        }
    }

    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);

    painter->drawEllipse(boundingRect());

    painter->restore();

    SourceGraphicsObject::paint(painter, option, widget);
}

bool CircleSumSourceGraphicsObject::otherPaint(QPainter *painter, const QRectF rect)
{
    painter->save();
    QSharedPointer<SourceProxy> sourceProxy = getSourceProxy();
    if (sourceProxy.isNull()) {
        return false;
    }
    painter->save();
    QPen pen;
    pen.setColor(sourceProxy->getStateColor());
    painter->setPen(pen);
    painter->setBrush(QBrush("#FFFFFF"));

    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter->drawEllipse(rect);
    painter->restore();
    return false;
}
