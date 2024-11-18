#include "ConnectorWireRegulationController.h"
#include "CanvasSceneDefaultImpl.h"
#include "ConnectorWireGraphicsObject.h"
#include "PortGraphicsObject.h"
#include "Utility.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QPainter>

ConnectorWireRegulationController::ConnectorWireRegulationController(ConnectorWireGraphicsObject *parent,
                                                                     QString portType)
    : QGraphicsObject(parent)
{
    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    connectorWire = parent;
    isHover = false;
    this->portType = portType;
    matchedController = nullptr;
    matchedPort = nullptr;
    regionSize = 10.f;
}

ConnectorWireRegulationController::~ConnectorWireRegulationController() { }

int ConnectorWireRegulationController::type() const
{
    return kConnectorRetulationControllerGraphics;
}

ConnectorWireRegulationController *ConnectorWireRegulationController::getMatchedRegulationController()
{
    return matchedController;
}

ConnectorWireGraphicsObject *ConnectorWireRegulationController::getConnectorWire()
{
    return connectorWire;
}

void ConnectorWireRegulationController::setHoverFlag(bool value)
{
    isHover = value;
}

void ConnectorWireRegulationController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                              QWidget *widget)
{
    if (!connectorWire || connectorWire->isConnectedWithPort() || matchedController) {
        return;
    }
    auto canvasContext = connectorWire->getCanvasContext();
    if (canvasContext && !canvasContext->isSupportSuspendedConnectorWire()) {
        return;
    }
    painter->save();
    painter->setPen(QPen(QColor(Qt::red), 1, Qt::SolidLine));
    QPointF centerPos = boundingRect().center();
    QPolygonF pts;
    float halfRegion = regionSize / 2;
    double offset = 0;
    if (portType == PortOutputType) {
        offset = halfRegion;
    }
    if (connectorWire) {
        auto dir = connectorWire->getRegulationControllerDirection(this);
        switch (dir) {
        case Utility::kRightToLeft: {
            pts.append(QPointF(centerPos.x() - offset, centerPos.y() - halfRegion));
            pts.append(QPointF(centerPos.x() + halfRegion - offset, centerPos.y()));
            pts.append(QPointF(centerPos.x() - offset, centerPos.y() + halfRegion));
            painter->drawPolyline(pts);

        } break;
        case Utility::kLeftToRight: {
            pts.append(QPointF(centerPos.x() + offset, centerPos.y() - halfRegion));
            pts.append(QPointF(centerPos.x() - halfRegion + offset, centerPos.y()));
            pts.append(QPointF(centerPos.x() + offset, centerPos.y() + halfRegion));
            painter->drawPolyline(pts);

        } break;
        case Utility::kBottomToTop: {
            pts.append(QPointF(centerPos.x() - halfRegion, centerPos.y() - offset));
            pts.append(QPointF(centerPos.x(), centerPos.y() + halfRegion - offset));
            pts.append(QPointF(centerPos.x() + halfRegion, centerPos.y() - offset));
            painter->drawPolyline(pts);

        } break;
        case Utility::kTopToBottom: {
            pts.append(QPointF(centerPos.x() - halfRegion, centerPos.y() + offset));
            pts.append(QPointF(centerPos.x(), centerPos.y() - halfRegion + offset));
            pts.append(QPointF(centerPos.x() + halfRegion, centerPos.y() + offset));
            painter->drawPolyline(pts);

        } break;
        default: {
            painter->drawEllipse(boundingRect());
        }
        }
    }

    painter->restore();
}

void ConnectorWireRegulationController::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    qDebug() << __FUNCTION__;
    isHover = true;
}

void ConnectorWireRegulationController::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    qDebug() << __FUNCTION__;
    isHover = false;
}

QVariant ConnectorWireRegulationController::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case QGraphicsItem::ItemPositionHasChanged: {
        QPointF currentPt = pos();
        processPosChanged(currentPt);
        break;
    }
    case QGraphicsItem::ItemPositionChange: {
        if (connectorWire) {
            auto canvasContext = connectorWire->getCanvasContext();
            if (canvasContext && !canvasContext->canModify()) {
                return pos();
            }
        }

    } break;
    default:
        break;
    }
    return QGraphicsObject::itemChange(change, value);
}

void ConnectorWireRegulationController::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    CanvasSceneDefaultImpl *canvasScene = dynamic_cast<CanvasSceneDefaultImpl *>(scene());
    if (!canvasScene) {
        return QGraphicsObject::mouseReleaseEvent(event);
    }
    if (connectorWire && connectorWire->isConnectedWithPort()) {
        isHover = false;
        setVisible(false);
        canvasScene->addConnectorWireGraphicsObject(connectorWire, true);
    }
    if (matchedController && matchedController->connectorWire) {
        canvasScene->deleteConnectorWireGraphicsObject(matchedController->connectorWire);
        matchedController = nullptr;
        matchedPort = nullptr;
    }
    QGraphicsObject::mouseReleaseEvent(event);
}

QRectF ConnectorWireRegulationController::boundingRect() const
{
    return QRectF(-regionSize / 2, -regionSize / 2, regionSize, regionSize);
}

void ConnectorWireRegulationController::processPosChanged(const QPointF &pt)
{
    QGraphicsScene *currentScene = scene();
    if (!currentScene || !connectorWire || !isHover) {
        return;
    }

    bool hasMatchPort = false;
    bool isWireConnected = connectorWire->isConnectedWithPort();
    auto outputPort = connectorWire->getOutputTypePortGraphics();
    auto inputPort = connectorWire->getInputTypePortGraphics();
    if (isWireConnected) {
        if (outputPort && !hasMatchPort) {
            QRectF portRect = outputPort->mapToScene(outputPort->boundingRect()).boundingRect();
            if (portRect.contains(pt)) {
                hasMatchPort = true;
            }
        }
        if (inputPort && !hasMatchPort) {
            QRectF portRect = inputPort->mapToScene(inputPort->boundingRect()).boundingRect();
            if (portRect.contains(pt)) {
                hasMatchPort = true;
            }
        }
    }
    if (hasMatchPort) {
        return;
    }

    QRectF detectionArea = QRectF(pt, QSize(8, 8));
    detectionArea.moveCenter(pt);
    QList<QGraphicsItem *> items = currentScene->items(detectionArea);

    foreach (QGraphicsItem *item, items) {
        int itemType = item->type();
        if (itemType == kPortGraphics) { // 命中图元为端口
            PortGraphicsObject *portGraphics = dynamic_cast<PortGraphicsObject *>(item);
            if (isWireConnected) {
                if (inputPort == portGraphics || outputPort == portGraphics) {
                    hasMatchPort = true;
                    break;
                }
            }
            if (portGraphics && portGraphics->canLink()) {
                if (!outputPort && !inputPort) {
                    // 当前连接线没有连接到任何端口，那么所有的端口都可以连接
                    connectorWire->linkPortGraphicsObject(portGraphics);
                    if (connectorWire->isConnectedWithPort()) {
                        hasMatchPort = true;
                        break;
                    }
                } else if (!inputPort) {
                    // 只能连接输入端口
                    auto checkPortContext = portGraphics->getPortContext();
                    if (checkPortContext && checkPortContext->type() == PortInputType) {
                        connectorWire->linkPortGraphicsObject(portGraphics);
                        if (connectorWire->isConnectedWithPort()) {
                            hasMatchPort = true;
                            break;
                        }
                    }
                } else if (!outputPort) {
                    // 只能连接输出端口
                    auto checkPortContext = portGraphics->getPortContext();
                    if (checkPortContext && checkPortContext->type() == PortOutputType) {
                        connectorWire->linkPortGraphicsObject(portGraphics);
                        if (connectorWire->isConnectedWithPort()) {
                            hasMatchPort = true;
                            break;
                        }
                    }
                }
            }
        } else if (itemType == kConnectorRetulationControllerGraphics) {
            ConnectorWireRegulationController *controller = dynamic_cast<ConnectorWireRegulationController *>(item);

            if (controller) {
                if (controller == this) {
                    continue;
                }
                if (controller == matchedController) {
                    hasMatchPort = true;
                    break;
                }
                QString controllerType = controller->portType;
                if (this->portType == controllerType) {
                    continue;
                }
                if (this->portType == PortInputType) {
                    auto inputPort = controller->connectorWire->getInputTypePortGraphics();
                    if (inputPort) {
                        controller->connectorWire->unlinkPortGraphicsObject(inputPort);
                        this->connectorWire->linkPortGraphicsObject(inputPort);
                        if (this->connectorWire->isConnectedWithPort()) {
                            matchedController = controller;
                            controller->matchedController = controller;
                            matchedPort = inputPort;
                            hasMatchPort = true;
                            break;
                        }
                    }
                } else if (this->portType == PortOutputType) {
                    auto outputPort = controller->connectorWire->getOutputTypePortGraphics();
                    if (outputPort) {
                        controller->connectorWire->unlinkPortGraphicsObject(outputPort);
                        this->connectorWire->linkPortGraphicsObject(outputPort);
                        if (this->connectorWire->isConnectedWithPort()) {
                            matchedController = controller;
                            controller->matchedController = controller;
                            matchedPort = outputPort;
                            hasMatchPort = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    if (!hasMatchPort) {
        emit positionChanged(pt);
        if (matchedController) {
            matchedController->connectorWire->linkPortGraphicsObject(matchedPort);
            matchedController->matchedController = nullptr;
        }
        matchedController = nullptr;
        matchedPort = nullptr;
    }
}

void ConnectorWireRegulationController::matchAnotherRegulationController(ConnectorWireRegulationController *controller)
{
    if (matchedController == controller) {
        return;
    }
    matchedController = controller;
}
