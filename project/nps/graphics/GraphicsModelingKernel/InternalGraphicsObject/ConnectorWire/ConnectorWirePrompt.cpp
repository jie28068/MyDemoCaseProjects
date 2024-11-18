#include "ConnectorWirePrompt.h"
#include "BezierConnectorWireAlgorithm.h"
#include "CanvasContext.h"
#include "ICanvasScene.h"
#include "ManhattaConnectorWireAlgorithm.h"
#include "PortGraphicsObject.h"
#include "StraightLineConnectorWireAlgorithm.h"

#include <QPainter>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QWeakPointer>

ConnectorWirePrompt::ConnectorWirePrompt(PortGraphicsObject *port1, PortGraphicsObject *port2, QGraphicsItem *parent)
    : QGraphicsObject(parent)
{
    setAcceptHoverEvents(true);
    setPorts(port1, port2);
    isHover = false;
}

ConnectorWirePrompt::~ConnectorWirePrompt() { }

void ConnectorWirePrompt::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!weakPort1.data() || !weakPort2.data()) {
        return;
    }

    painter->restore();

    QColor penColor(0x2E9FE6);
    if (!isHover) {
        penColor.setAlpha(128);
    }

    QPen pen(penColor);

    int penWidth = 2;
    pen.setWidth(penWidth);

    painter->setPen(pen);

    QPointF drawInputPos = inputPos;

    QPolygonF arrowPoints;
    arrowPoints.push_back(inputPos);
    int len = 8;
    int offset = 5;
    // 绘制三角形箭头
    switch (inputAnchor) {
    case Qt::AnchorLeft:
        drawInputPos = QPointF(inputPos.x() - len, inputPos.y());
        arrowPoints.push_back(QPointF(inputPos.x() - len, inputPos.y() - offset));
        arrowPoints.push_back(QPointF(inputPos.x() - len, inputPos.y() + offset));
        break;
    case Qt::AnchorTop:
        drawInputPos = QPointF(inputPos.x(), inputPos.y() - len);
        arrowPoints.push_back(QPointF(inputPos.x() - offset, inputPos.y() - len));
        arrowPoints.push_back(QPointF(inputPos.x() + offset, inputPos.y() - len));
        break;
    case Qt::AnchorRight:
        drawInputPos = QPointF(inputPos.x() + len, inputPos.y());
        arrowPoints.push_back(QPointF(inputPos.x() + len, inputPos.y() - offset));
        arrowPoints.push_back(QPointF(inputPos.x() + len, inputPos.y() + offset));
        break;
    case Qt::AnchorBottom:
        drawInputPos = QPointF(inputPos.x(), inputPos.y() + len);
        arrowPoints.push_back(QPointF(inputPos.x() - offset, inputPos.y() + len));
        arrowPoints.push_back(QPointF(inputPos.x() + offset, inputPos.y() + len));
        break;
    }

    if (points.size() > 0) {
        painter->drawPolyline(mapFromScene(points));
    }

    // painter->drawLine(outputPos, drawInputPos);

    QPainterPath path;
    pen.setWidth(1);
    // pen.setCosmetic(true);
    painter->setPen(pen);
    path.addPolygon(mapFromScene(arrowPoints));
    path.closeSubpath();
    painter->fillPath(path, penColor);

    painter->save();
}

QRectF ConnectorWirePrompt::boundingRect() const
{
    return shape().controlPointRect();
}

QList<QWeakPointer<PortGraphicsObject>> ConnectorWirePrompt::getPortList()
{
    QList<QWeakPointer<PortGraphicsObject>> portList;
    portList.push_back(weakPort1);
    portList.push_back(weakPort2);
    return portList;
}

void ConnectorWirePrompt::setPorts(PortGraphicsObject *port1, PortGraphicsObject *port2)
{
    weakPort1 = QWeakPointer<PortGraphicsObject>(port1);
    weakPort2 = QWeakPointer<PortGraphicsObject>(port2);
    if (port1 && port2) {

        IConnectorWireAlgorithm *algorithm = nullptr;
        QString algoritmName = ManhattaAlgorithmName;
        if (StraightLineAlgorithmName == algoritmName) {
            algorithm = new StraightLineConnectorWireAlgorithm();
        } else if (ManhattaAlgorithmName == algoritmName) {
            algorithm = new ManhattaConnectorWireAlgorithm();
        } else if (BezierAlogrithmName == algoritmName) {
            algorithm = new BezierConnectorWireAlgorithm();
        } else {
            algorithm = new ManhattaConnectorWireAlgorithm();
        }

        QPainterPath obscalePath;
        auto canvasScene = port1->getCanvasScene();
        if (canvasScene) {
            obscalePath = canvasScene->getTransformProxyGraphicsPainterPath();
        }

        if (!port1->isOutputPort()) {
            points = algorithm->getAlgorithmPath(port2->getLinkPos(), port2->getExternalPos(), port1->getLinkPos(),
                                                 port1->getExternalPos(), obscalePath);
            inputPos = port1->getPropmtPos();
            inputAnchor = port1->getAnchorPoint();
            outputPos = port2->getPropmtPos();
        } else {
            points = algorithm->getAlgorithmPath(port1->getLinkPos(), port1->getExternalPos(), port2->getLinkPos(),
                                                 port2->getExternalPos(), obscalePath);
            inputPos = port2->getPropmtPos();
            inputAnchor = port2->getAnchorPoint();
            outputPos = port1->getPropmtPos();
        }

        if (points.size() >= 4) {
            points[0] = outputPos;
            points[points.size() - 1] = inputPos;
        } else {
            points = QPolygonF();
        }

        if (algorithm) {
            delete algorithm;
            algorithm = nullptr;
        }
    } else {
        points = QPolygonF();
        inputPos = QPointF();
        inputAnchor = Qt::AnchorLeft;
        outputPos = QPointF();
    }
}

QPainterPath ConnectorWirePrompt::shape() const
{
    QPainterPath path;
    if (!weakPort1.data() || !weakPort2.data()) {
        return path;
    }
    QPointF port1Pos = weakPort1.data()->getExternalPos();
    QPointF port2Pos = weakPort2.data()->getExternalPos();
    path.moveTo(port1Pos);
    path.lineTo(port2Pos);
    QPainterPathStroker stroker;
    stroker.setWidth(4);
    path = stroker.createStroke(path);
    return path;
}

void ConnectorWirePrompt::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    isHover = true;
    QGraphicsObject::hoverEnterEvent(event);
}

void ConnectorWirePrompt::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    isHover = false;
    QGraphicsObject::hoverLeaveEvent(event);
}

void ConnectorWirePrompt::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    if (weakPort1.data() && weakPort2.data()) {
        emit clicked(weakPort1.data(), weakPort2.data());
        setVisible(false);
    }
    QGraphicsObject::mousePressEvent(event);
}
