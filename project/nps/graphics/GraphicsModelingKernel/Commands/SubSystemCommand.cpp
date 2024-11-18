#include "SubSystemCommand.h"

SubSystemCommand::SubSystemCommand(ICanvasScene *scene, QUndoCommand *parent) : CanvasUndoCommand(scene, parent) { }

SubSystemCommand::~SubSystemCommand() { }

void SubSystemCommand::setSelectedRect(const QRectF &rect)
{
    selectedRect = rect;
}

void SubSystemCommand::redo()
{

    CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(canvasScene);
    if (defaultScene) {
        auto canvasContext = defaultScene->getCanvasContext();
        if (!canvasContext) {
            return;
        }

        BusinessHooksServer *businessHook = canvasContext->getBusinessHooksServer();
        if (!businessHook) {
            return;
        }
        defaultScene->getUndoStack()->setIgnoreCommandFlag(true);

        if (subsystemSourceID == "") {
            auto items = defaultScene->selectedItems();

            QRectF selectedSourceRect;

            foreach (auto item, items) {
                GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
                if (layer) {
                    if (layer->type() == kTransformProxyGraphics) {
                        TransformProxyGraphicsObject *proxyGraphics =
                                dynamic_cast<TransformProxyGraphicsObject *>(item);
                        if (proxyGraphics) {
                            auto sourceProxy = proxyGraphics->getSourceProxy();
                            if (!sourceProxy) {
                                continue;
                            }
                            bool isAnnotaionModule = (sourceProxy->moduleType() == GKD::SOURCE_MODULETYPE_ANNOTATION);
                            if (isAnnotaionModule) {
                                continue;
                            }
                            selectedSources.append(sourceProxy);
                            selectedSourceRect = selectedSourceRect.united(proxyGraphics->getTransformSceneRect());

                            auto portGraphics = proxyGraphics->getPortGraphicsObjectList();
                            foreach (auto port, portGraphics) {
                                QList<ConnectorWireGraphicsObject *> linkedWires = port->getLinkedConnectorWireList();
                                foreach (auto wire, linkedWires) {
                                    auto inputPort = wire->getInputTypePortGraphics();
                                    auto outputPort = wire->getOutputTypePortGraphics();
                                    auto wireContext = wire->getConnectorWireContext();
                                    if (inputPort && outputPort) {
                                        TransformProxyGraphicsObject *inputProxyGraphics =
                                                inputPort->getTransformProxyGraphicsObject();
                                        TransformProxyGraphicsObject *outputProxyGraphics =
                                                outputPort->getTransformProxyGraphicsObject();
                                        if (inputProxyGraphics->isSelected() && outputProxyGraphics->isSelected()) {
                                            if (!doubleSelectedWireContext.contains(wireContext)) {
                                                doubleSelectedWireContext.append(wireContext);
                                            }
                                        } else {
                                            if (!singleSideSelectedWireContext.contains(wireContext)) {
                                                singleSideSelectedWireContext.append(wireContext);
                                            }
                                        }
                                    } else {
                                        // 悬空的连接线，直接删除
                                        defaultScene->deleteConnectorWireGraphicsObject(wire);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (!selectedSourceRect.isNull()) {
                selectedRect = selectedSourceRect;
            }
        }

        if (!selectedSources.isEmpty()) {
            QMap<QString, QString> newLinkMap;
            businessHook->createContructorSubSystem(selectedRect, selectedSources, doubleSelectedWireContext,
                                                    singleSideSelectedWireContext, subsystemSourceID, newLinkMap);
            foreach (auto wireCtx, doubleSelectedWireContext) {
                ConnectorWireGraphicsObject *connectorGraphics =
                        defaultScene->getConnectorWireGraphicsByID(wireCtx->uuid());
                QString newLinkPortName = newLinkMap.value(wireCtx->uuid(), "");
                if (!newLinkPortName.isEmpty()) {
                    TransformProxyGraphicsObject *subsystemGraphics =
                            defaultScene->getTransformProxyGraphicsByID(subsystemSourceID);
                    if (subsystemGraphics) {
                        auto ports = subsystemGraphics->getPortGraphicsObjectList();
                        foreach (auto port, ports) {
                            if (port && port->getPortContext()->name() == newLinkPortName) {
                                connectorGraphics->linkPortGraphicsObject(port);
                                break;
                            }
                        }
                    }
                } else {
                    defaultScene->deleteConnectorWireGraphicsObject(connectorGraphics);
                }
            }

            foreach (auto wireCtx, singleSideSelectedWireContext) {
                ConnectorWireGraphicsObject *connectorGraphics =
                        defaultScene->getConnectorWireGraphicsByID(wireCtx->uuid());
                defaultScene->deleteConnectorWireGraphicsObject(connectorGraphics);
                QString newLinkPortName = newLinkMap.value(wireCtx->uuid(), "");

                PConnectorWireContext copyWireContext = PConnectorWireContext(new ConnectorWireContext(*wireCtx));

                bool isSrcChanged = false;
                auto srcSource = copyWireContext->srcSource();
                auto dstSource = copyWireContext->dstSource();
                auto srcTransformGraphics = defaultScene->getTransformProxyGraphicsBySource(srcSource);
                auto dstTransformGraphics = defaultScene->getTransformProxyGraphicsBySource(dstSource);
                if (srcTransformGraphics && dstTransformGraphics) {
                    continue;
                }
                isSrcChanged = srcTransformGraphics == nullptr ? true : false;

                TransformProxyGraphicsObject *subsystemGraphics =
                        defaultScene->getTransformProxyGraphicsByID(subsystemSourceID);
                if (subsystemGraphics && !newLinkPortName.isEmpty()) {
                    auto ports = subsystemGraphics->getPortGraphicsObjectList();
                    foreach (auto port, ports) {
                        if (port && port->getPortContext()->name() == newLinkPortName) {
                            if (isSrcChanged) {
                                copyWireContext->setSrcSource(subsystemGraphics->getSourceProxy());
                                copyWireContext->setSrcPortID(port->getPortContext()->uuid());
                            } else {
                                copyWireContext->setDstSource(subsystemGraphics->getSourceProxy());
                                copyWireContext->setDstPortID(port->getPortContext()->uuid());
                            }
                            ConnectorWireGraphicsObject *graphics = new ConnectorWireGraphicsObject(canvasScene);
                            graphics->loadFromContext(copyWireContext);
                            // 连接成功，把连接线上下文添加到画板,这里需要触发数据同步
                            if (graphics->isConnectedWithPort()) {
                                canvasContext->addConnectWireContext(copyWireContext);
                            }
                            break;
                        }
                    }
                }
            }

        } else {
            businessHook->createEmptySubsystem(selectedRect, subsystemSourceID);
        }
        businessHook->renameSubsystem(subsystemSourceID);
        defaultScene->getUndoStack()->setIgnoreCommandFlag(false);
    }
}

void SubSystemCommand::undo()
{
    CanvasSceneDefaultImpl *defaultScene = dynamic_cast<CanvasSceneDefaultImpl *>(canvasScene);
    if (defaultScene) {
        TransformProxyGraphicsObject *subsystemGraphics =
                defaultScene->getTransformProxyGraphicsByID(subsystemSourceID);
        if (!subsystemGraphics) {
            return;
        }
        auto canvasContext = defaultScene->getCanvasContext();
        if (!canvasContext) {
            return;
        }
        BusinessHooksServer *businessHook = canvasContext->getBusinessHooksServer();
        if (!businessHook) {
            return;
        }
        defaultScene->setLoadingCanvasStatus(true);
        defaultScene->getUndoStack()->setIgnoreCommandFlag(true);
        /*  if (!selectedSources.isEmpty()) {
             businessHook->destroyContructorSubSystem(subsystemSourceID, selectedSources);
         } */

        // 把子系统模块上的连线清除
        QList<PortGraphicsObject *> ports = subsystemGraphics->getPortGraphicsObjectList();
        foreach (auto port, ports) {
            auto wires = port->getLinkedConnectorWireList();
            foreach (auto wire, wires) {
                defaultScene->deleteConnectorWireGraphicsObject(wire);
            }
        }
        defaultScene->deleteTransformProxyGraphicsObject(subsystemGraphics);

        foreach (auto source, selectedSources) {
            if (defaultScene->getTransformProxyGraphicsByID(source->uuid()) != nullptr) {
                continue;
            }
            source->setSourceCreateType(SourceProxy::kCreateRevert);
            canvasScene->loadFromSourceProxy(source);
        }

        foreach (auto wire, doubleSelectedWireContext) {
            if (defaultScene->getConnectorWireGraphicsByID(wire->uuid()) != nullptr) {
                continue;
            }
            ConnectorWireGraphicsObject *graphics = new ConnectorWireGraphicsObject(canvasScene);
            graphics->loadFromContext(wire);
            // 连接成功，把连接线上下文添加到画板,这里需要触发数据同步
            if (graphics->isConnectedWithPort()) {
                canvasContext->addConnectWireContext(wire);
            }
        }

        foreach (auto wire, singleSideSelectedWireContext) {
            if (defaultScene->getConnectorWireGraphicsByID(wire->uuid()) != nullptr) {
                continue;
            }
            ConnectorWireGraphicsObject *graphics = new ConnectorWireGraphicsObject(canvasScene);
            graphics->loadFromContext(wire);
            // 连接成功，把连接线上下文添加到画板,这里需要触发数据同步
            if (graphics->isConnectedWithPort()) {
                canvasContext->addConnectWireContext(wire);
            }
        }
        defaultScene->getUndoStack()->setIgnoreCommandFlag(false);
        defaultScene->setLoadingCanvasStatus(false);
        defaultScene->clearSelection();
    }
}
