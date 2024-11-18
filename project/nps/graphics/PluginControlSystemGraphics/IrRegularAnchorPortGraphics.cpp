#include "IrRegularAnchorPortGraphics.h"

#include <qmath.h>

IrRegularAnchorPortGraphics::IrRegularAnchorPortGraphics(ICanvasScene *canvasScene,
                                                         SourceGraphicsObject *sourceGraphics,
                                                         QSharedPointer<PortContext> context, QGraphicsItem *parent)
    : PortGraphicsObject(canvasScene, sourceGraphics, context, parent)
{
    radius = 10.f;
    portNameGraphics = nullptr;
    dataTextGraphics = nullptr;
    connect(context.data(), SIGNAL(onPortHideOrShow(bool)), this, SLOT(onPortHideOrShowChange(bool)));
}

IrRegularAnchorPortGraphics::~IrRegularAnchorPortGraphics() { }

void IrRegularAnchorPortGraphics::createInternalGraphics()
{
    if (nullptr == portNameGraphics) {
        portNameGraphics = new IrRegularPortNameTextGraphicsObject(getCanvasScene(), this);

        // 控制模块显示端口名称时，使用端口中文名，如果中文名为空，则显示变量名称
        if (getPortContext()->displayName() != "") {
            portNameGraphics->setPlainText(getPortContext()->displayName());
        } else {
            portNameGraphics->setPlainText(getPortContext()->name());
        }
        portNameGraphics->updatePosition();

        portNameGraphics->userShow(getPortContext()->isShowName());
    }
    if (getPortContext()->type() == PortOutputType) {
        dataTextGraphics = new ControlDataTextGraphicsObject(getCanvasScene(), this);
        dataTextGraphics->userShow(false);
    }
}

Qt::AnchorPoint IrRegularAnchorPortGraphics::getAnchorPoint()
{

    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    int angle = getPortContext()->getAngle();
    int rotateAngle = proxyGraphics->getAngle();
    angle += rotateAngle;
    angle = angle % 360;
    if (angle >= 315 || angle <= 45) {
        return Qt::AnchorRight;
    } else if (angle > 45 && angle <= 135) {
        return Qt::AnchorBottom;
    } else if (angle > 135 && angle <= 225) {
        return Qt::AnchorLeft;
    } else if (angle > 225 && angle < 315) {
        return Qt::AnchorTop;
    }
    return PortGraphicsObject::getAnchorPoint();
}

void IrRegularAnchorPortGraphics::updatePosition()
{

    int arrowAngle = 25;
    TransformProxyGraphicsObject *proxyGraphics = getTransformProxyGraphicsObject();
    int angle = getPortContext()->getAngle();
    int rotateAngle = proxyGraphics->getAngle();
    angle += rotateAngle;
    currentAngle = angle;
    QRectF proxyRc = getTransformProxyGraphicsObject()->boundingRect();
    QPointF pt = Utility::calcAnglePoint(angle, proxyRc);
    setPos(pt);
    if (getPortContext()->type() == PortInputType) {
        arrowPoints.clear();

        arrowPoints = Utility::getArrorwPoint(mapFromScene(getExternalPos()),
                                              mapFromScene(PortGraphicsObject::getLinkPos()), 25.f / 57.3, 7);
    } else {
        arrowPoints.clear();

        arrowPoints = Utility::getArrorwPoint(
                mapFromScene(PortGraphicsObject::getLinkPos()),
                Utility::calcAnglePoint(currentAngle, boundingRect().adjusted(-2, -2, 2, 2)), 25.f / 57.3, 7);
    }
    GraphicsLayer::updatePosition();
}

QRectF IrRegularAnchorPortGraphics::boundingRect() const
{
    return QRectF(-radius / 2.f, -radius / 2.f, radius, radius);
}

QPainterPath IrRegularAnchorPortGraphics::shape() const
{
    QPainterPath path;
    path.addPolygon(arrowPoints);
    path.closeSubpath();

    // QPainterPathStroker stroker;
    //// 此处加10是为了扩大检测区域，使得图元容易被命中
    // stroker.setWidth(2);
    // path = stroker.createStroke(path);
    return path;
}

QPointF IrRegularAnchorPortGraphics::getLinkPos()
{
    return PortGraphicsObject::getLinkPos();
    if (getPortContext()->type() == PortInputType) {
        return PortGraphicsObject::getLinkPos();
    } else {
        return mapToScene(Utility::calcAnglePoint(currentAngle, boundingRect().adjusted(-2, -2, 2, 2)));
    }
    return QPointF();
}

QPointF IrRegularAnchorPortGraphics::getExternalPos()
{
    qreal externalRadius = radius + 30;
    QRectF rc = QRectF(-externalRadius / 2.f, -externalRadius / 2.f, externalRadius, externalRadius);
    QPointF pt1 = Utility::calcAnglePoint(currentAngle, rc);
    return mapToScene(pt1);
}

bool IrRegularAnchorPortGraphics::canMove()
{
    return false;
}

QPointF IrRegularAnchorPortGraphics::getPortNamePos()
{
    qreal externalRadius = radius - 22;
    QRectF rc = QRectF(-externalRadius / 2.f, -externalRadius / 2.f, externalRadius, externalRadius);
    QPointF pt1 = Utility::calcAnglePoint(currentAngle, rc);
    return mapToScene(pt1);
    return mapToScene(Utility::calcAnglePoint(currentAngle, boundingRect().adjusted(5, 5, 5, 5)));
}

void IrRegularAnchorPortGraphics::setDataTextShow(const QString &text)
{
    if (dataTextGraphics) {
        if (text == QString()) {
            this->setZValue(0);
            dataTextGraphics->setZValue(0);
            dataTextGraphics->userShow(false);
            dataTextGraphics->setPlainText("");
        } else {
            getTransformProxyGraphicsObject()->setZValue(100);
            this->setZValue(100);
            dataTextGraphics->setZValue(100);
            dataTextGraphics->userShow(true);
            dataTextGraphics->setPlainText(text);
            updatePosition();
        }
    }
}

void IrRegularAnchorPortGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!getLinkedConnectorWireList().isEmpty()) {
        return;
    }
    painter->save();

    QPen pen;
    pen.setWidth(1);
    pen.setColor(GKD::WIRE_NORMAL_COLOR);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);
    painter->drawPolyline(arrowPoints);

    painter->restore();
}

void IrRegularAnchorPortGraphics::onPortHideOrShowChange(bool value)
{
    if (portNameGraphics) {
        portNameGraphics->userShow(value);
    }
}