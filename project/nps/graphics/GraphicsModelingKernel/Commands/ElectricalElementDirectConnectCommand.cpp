#include "ElectricalElementDirectConnectCommand.h"

ElectricalElementDirectConnectCommand::ElectricalElementDirectConnectCommand(ICanvasScene *scene, QUndoCommand *parent)
    : CanvasUndoCommand(scene, parent)
{
}

ElectricalElementDirectConnectCommand::~ElectricalElementDirectConnectCommand() { }

void ElectricalElementDirectConnectCommand::setElementInfos(QPointF dotbusBarPos, ElectricalElementInfo info1,
                                                            ElectricalElementInfo info2)
{
    busbarPos = dotbusBarPos;
    electricalElementInfo1 = info1;
    electricalElementInfo2 = info2;
}

void ElectricalElementDirectConnectCommand::redo()
{
    CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(canvasScene);
    if (!defaultScene) {
        return;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }
    BusinessHooksServer *hookserver = canvasContext->getBusinessHooksServer();
    if (!hookserver) {
        return;
    }

    TransformProxyGraphicsObject *source1 =
            canvasScene->getTransformProxyGraphicsByID(electricalElementInfo1.sourceUUID);
    TransformProxyGraphicsObject *source2 =
            canvasScene->getTransformProxyGraphicsByID(electricalElementInfo2.sourceUUID);
    if (!source1 || !source2) {
        return;
    }
    PortGraphicsObject *port1 = source1->getPortGraphicsObject(electricalElementInfo1.portUUID);
    PortGraphicsObject *port2 = source2->getPortGraphicsObject(electricalElementInfo2.portUUID);
    if (!port1 || !port2) {
        return;
    }
    defaultScene->getUndoStack()->setIgnoreCommandFlag(true);

    QString dotBusbarUUID = hookserver->createNewBlock("DotBusbar", busbarPos, false, QString(), lastBusbarUUID);

    TransformProxyGraphicsObject *busbarProxyGraphics = canvasScene->getTransformProxyGraphicsByID(dotBusbarUUID);
    if (!busbarProxyGraphics) {
        defaultScene->getUndoStack()->setIgnoreCommandFlag(false);
        return;
    }
    lastBusbarUUID = dotBusbarUUID;

    busbarSourceProxy = busbarProxyGraphics->getSourceProxy();
    SourceGraphicsObject *busbarSourceGraphics = busbarProxyGraphics->getSourceGraphicsObject();

    if (busbarSourceGraphics && busbarSourceProxy) {
        PortGraphicsObject *newBusbarPort1 = busbarSourceGraphics->addPortGraphicsObject(busbarPos);
        ConnectorWireGraphicsObject *newWire1 = new ConnectorWireGraphicsObject(canvasScene, nullptr);
        if (!lastWireUUID1.isEmpty() && !canvasContext->getConnectorWireContext(lastWireUUID1)) {
            newWire1->getConnectorWireContext()->setUUID(lastWireUUID1);
        }
        newWire1->linkPortGraphicsObject(port1);
        newWire1->linkPortGraphicsObject(newBusbarPort1);

        PortGraphicsObject *newBusbarPort2 = busbarSourceGraphics->addPortGraphicsObject(busbarPos);
        ConnectorWireGraphicsObject *newWire2 = new ConnectorWireGraphicsObject(canvasScene, nullptr);
        if (!lastWireUUID2.isEmpty() && !canvasContext->getConnectorWireContext(lastWireUUID2)) {
            newWire2->getConnectorWireContext()->setUUID(lastWireUUID2);
        }
        newWire2->linkPortGraphicsObject(port2);
        newWire2->linkPortGraphicsObject(newBusbarPort2);

        if (newWire1->isConnectedWithPort() && newWire2->isConnectedWithPort()) {
            // 两个电气元件都连接到了新创建的点状母线
            connectorWireContext1 = newWire1->getConnectorWireContext();
            connectorWireContext2 = newWire2->getConnectorWireContext();
            canvasScene->addConnectorWireGraphicsObject(newWire1);
            canvasScene->addConnectorWireGraphicsObject(newWire2);
            canvasContext->addConnectWireContext(connectorWireContext1);
            canvasContext->addConnectWireContext(connectorWireContext2);

            // 设置点状母线的类型
            QString port1Type =
                    hookserver->getElectricalPortType(source1->getSourceProxy(), electricalElementInfo1.portUUID);
            QString port2Type =
                    hookserver->getElectricalPortType(source2->getSourceProxy(), electricalElementInfo2.portUUID);
            if (port1Type == "DC" || port2Type == "DC") {
                // dc
                hookserver->setElectricalACDCType(busbarSourceProxy, 0);
            } else {
                // ac
                hookserver->setElectricalACDCType(busbarSourceProxy, 1);
            }
            defaultScene->clearSelection();
            lastWireUUID1 = connectorWireContext1->uuid();
            lastWireUUID2 = connectorWireContext2->uuid();
        } else {
            newWire1->clearLinks();
            newWire2->clearLinks();
            lastWireUUID1 = "";
            lastWireUUID2 = "";
            canvasScene->deleteTransformProxyGraphicsObject(busbarProxyGraphics);
            delete newWire1;
            delete newWire2;

            busbarSourceProxy.reset();
        }
    }
    defaultScene->getUndoStack()->setIgnoreCommandFlag(false);
    startVerifyCanvas();
}

void ElectricalElementDirectConnectCommand::undo()
{
    CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(canvasScene);
    if (!defaultScene) {
        return;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }
    BusinessHooksServer *hookserver = canvasContext->getBusinessHooksServer();
    if (!hookserver) {
        return;
    }

    if (!busbarSourceProxy || !connectorWireContext1 || !connectorWireContext2) {
        return;
    }
    defaultScene->getUndoStack()->setIgnoreCommandFlag(true);

    canvasContext->deleteConnectWireContext(connectorWireContext1->uuid());
    canvasContext->deleteConnectWireContext(connectorWireContext2->uuid());
    canvasContext->deleteSourceProxy(busbarSourceProxy->uuid());

    busbarSourceProxy.reset();
    connectorWireContext1.reset();
    connectorWireContext2.reset();
    defaultScene->getUndoStack()->setIgnoreCommandFlag(false);
    startVerifyCanvas();
}
