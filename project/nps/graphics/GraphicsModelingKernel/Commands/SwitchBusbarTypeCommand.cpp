#include "SwitchBusbarTypeCommand.h"
#include "BusinessHooksServer.h"
#include "ICanvasScene.h"
#include "ICanvasView.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"

SwitchBusbarTypeCommand::SwitchBusbarTypeCommand(ICanvasScene *scene, QUndoCommand *parent)
    : CanvasUndoCommand(scene, parent)
{
}

SwitchBusbarTypeCommand::~SwitchBusbarTypeCommand() { }

void SwitchBusbarTypeCommand::setSourcePorxy(PSourceProxy sourceProxy)
{
    ICanvasView *canvasView = canvasScene->getCanvasView();
    if (!canvasView) {
        return;
    }
    auto proxyGraphics = canvasScene->getTransformProxyGraphicsByID(sourceProxy->uuid());
    if (!proxyGraphics) {
        return;
    }
    QString prototypeName = sourceProxy->prototypeName();
    originalSourceProxy = PSourceProxy(new SourceProxy(*sourceProxy.data()));
    newSourceProxy = PSourceProxy(new SourceProxy(*sourceProxy.data()));

    auto ports = newSourceProxy->portList();
    foreach (auto port, ports) {
        // 因为不是同类型的模型复制，需要把端口的originalUUID设置为空
        port->setOriginalPortUUID("");
    }

    TransformProxyGraphicsObject *originalProxyGraphics =
            canvasScene->getTransformProxyGraphicsByID(originalSourceProxy->uuid());

    newSourceProxy->setScaleTransform(QTransform());
    newSourceProxy->setRotateTransform(QTransform());
    newSourceProxy->setTranslateTransform(QTransform());
    newSourceProxy->setUUID(Utility::createUUID());
    if (prototypeName == "Busbar") {
        newSourceProxy->setPrototypeName("DotBusbar");
        foreach (auto port, newSourceProxy->portList()) {
            port->setPos(QPointF(0, 0));
        }
        QPointF pos = proxyGraphics->mapRectToScene(proxyGraphics->boundingRect()).center();
        newSourceProxy->setPos(pos);
        newSourceProxy->setSize(QSize(10, 10));
    } else if (prototypeName == "DotBusbar") {
        newSourceProxy->setPrototypeName("Busbar");
        foreach (auto port, newSourceProxy->portList()) {
            if (port->pos() == QPointF(0, 0)) {
                port->setPos(QPointF(50, 0));
            }
        }
        QPointF pos = newSourceProxy->pos() - QPointF(50, 0);
        newSourceProxy->setPos(pos);
        newSourceProxy->setSize(QSize(100, 10));
    }

    qDebug() << "设置参数时原始母线id:" << originalSourceProxy->uuid()
             << "切换新的母线/点状母线id:" << newSourceProxy->uuid();
}

void SwitchBusbarTypeCommand::redo()
{
    if (originalSourceProxy.isNull() || newSourceProxy.isNull() || !canvasScene) {
        return;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }
    qDebug() << "重做时原始母线id:" << originalSourceProxy->uuid()
             << "切换新的母线/点状母线id:" << newSourceProxy->uuid();
    ICanvasView *canvasView = canvasScene->getCanvasView();
    if (!canvasView) {
        return;
    }
    // 2023.06.19 by liwenyu bug: https://zt.xtkfpt.online/bug-view-8137.html
    // 此处需要根据保存的信息新创建一个对象，用来创建画板图元。因为后面处理母线的连接线的时候，切换端口连接会导致母线端口删除，从而触发sourceProxy的端口删除同步
    // 那么在撤销重做时，会导致端口信息被清空的情况，从而导致后续处理连接线的时候找不到端口信息
    auto createSourceProxy = PSourceProxy(new SourceProxy(*newSourceProxy.data()));
    createSourceProxy->setSourceCreateType(SourceProxy::kCreateNormal);
    canvasView->addSource(createSourceProxy, false);

    // 2023.08.23 by txy 外部业务相关,切换母线,将原母线的关联关系绑定到新的母线上
    if (canvasContext->getBusinessHooksServer()) {
        canvasContext->getBusinessHooksServer()->copyModelAssociationRelation(
                canvasContext->uuid(), originalSourceProxy->uuid(), newSourceProxy->uuid());
    }

    // 把连接线给连上
    TransformProxyGraphicsObject *originalProxyGraphics =
            canvasScene->getTransformProxyGraphicsByID(originalSourceProxy->uuid());
    TransformProxyGraphicsObject *newProxyGraphicsObject =
            canvasScene->getTransformProxyGraphicsByID(newSourceProxy->uuid());
    if (newProxyGraphicsObject && originalProxyGraphics) {
        if (newSourceProxy->prototypeName() == "Busbar") {
            newProxyGraphicsObject->applyTransformChanged();
        }

        auto originalSourceProxyBak = PSourceProxy(new SourceProxy(*originalSourceProxy.data()));
        auto copyPortList = newProxyGraphicsObject->getPortGraphicsObjectList();
        auto portList = originalProxyGraphics->getPortGraphicsObjectList();
        for each (auto port in portList) {
            if (!port) {
                continue;
            }
            auto portContext = port->getPortContext();
            if (!portContext) {
                continue;
            }
            auto wires = port->getLinkedConnectorWireList();
            if (wires.isEmpty()) {
                continue;
            }

            for each (auto wire in wires) {
                if (wire) {
                    QString portUUID = portContext->uuid();
                    PortGraphicsObject *newCopyPortGraphicsObject = nullptr;
                    for each (auto copyPort in copyPortList) {
                        if (!copyPort) {
                            continue;
                        }
                        auto copyPortContext = copyPort->getPortContext();
                        if (copyPortContext && copyPortContext->uuid() == portUUID) {
                            newCopyPortGraphicsObject = copyPort;
                            break;
                        }
                    }
                    if (newCopyPortGraphicsObject) {
                        auto wireCtx = wire->getConnectorWireContext();
                        if (wireCtx) {
                            wireCtx->setAutoCalculation(true);
                        }
                        wire->linkPortGraphicsObject(newCopyPortGraphicsObject);
                    }
                }
            }
        }
        originalSourceProxy = originalSourceProxyBak;
        auto businessHook = canvasContext->getBusinessHooksServer();
        if (businessHook != nullptr) {
            businessHook->copyModelVariableGroups(originalSourceProxy, newSourceProxy);
        }
        canvasScene->deleteTransformProxyGraphicsObject(originalProxyGraphics);
    }
    startVerifyCanvas();
}

void SwitchBusbarTypeCommand::undo()
{
    if (originalSourceProxy.isNull() || newSourceProxy.isNull() || !canvasScene) {
        return;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }
    qDebug() << "撤销时原始母线id:" << newSourceProxy->uuid()
             << "切换新的母线/点状母线id:" << originalSourceProxy->uuid();
    ICanvasView *canvasView = canvasScene->getCanvasView();
    if (!canvasView) {
        return;
    }

    // 2023.06.19 by liwenyu bug: https://zt.xtkfpt.online/bug-view-8137.html
    // 此处需要根据保存的信息新创建一个对象，用来创建画板图元。因为后面处理母线的连接线的时候，切换端口连接会导致母线端口删除，从而触发sourceProxy的端口删除同步
    // 那么在撤销重做时，会导致端口信息被清空的情况，从而导致后续处理连接线的时候找不到端口信息
    auto createSourceProxy = PSourceProxy(new SourceProxy(*originalSourceProxy.data()));
    createSourceProxy->setSourceCreateType(SourceProxy::kCreateNormal);
    canvasView->addSource(createSourceProxy, false);

    // 2023.08.23 by txy 外部业务相关,切换母线,将原母线的关联关系绑定到新的母线上
    if (canvasContext->getBusinessHooksServer()) {
        canvasContext->getBusinessHooksServer()->copyModelAssociationRelation(
                canvasContext->uuid(), newSourceProxy->uuid(), originalSourceProxy->uuid());
    }

    // 把连接线给连上
    TransformProxyGraphicsObject *originalProxyGraphics =
            canvasScene->getTransformProxyGraphicsByID(originalSourceProxy->uuid());
    TransformProxyGraphicsObject *newProxyGraphicsObject =
            canvasScene->getTransformProxyGraphicsByID(newSourceProxy->uuid());
    if (newProxyGraphicsObject && originalProxyGraphics) {
        if (createSourceProxy->prototypeName() == "Busbar") {
            originalProxyGraphics->applyTransformChanged();
        }
        auto newSourceProxyBak = PSourceProxy(new SourceProxy(*newSourceProxy.data()));
        auto originalPortList = originalProxyGraphics->getPortGraphicsObjectList();
        auto portList = newProxyGraphicsObject->getPortGraphicsObjectList();
        for each (auto port in portList) {
            if (!port) {
                continue;
            }
            auto portContext = port->getPortContext();
            if (!portContext) {
                continue;
            }
            auto wires = port->getLinkedConnectorWireList();
            if (wires.isEmpty()) {
                continue;
            }
            for each (auto wire in wires) {
                if (wire) {
                    QString portUUID = portContext->uuid();
                    PortGraphicsObject *newCopyPortGraphicsObject = nullptr;
                    for each (auto copyPort in originalPortList) {
                        if (!copyPort) {
                            continue;
                        }
                        auto copyPortContext = copyPort->getPortContext();
                        if (copyPortContext && copyPortContext->uuid() == portUUID) {
                            newCopyPortGraphicsObject = copyPort;
                            break;
                        }
                    }
                    if (newCopyPortGraphicsObject) {
                        auto wireContext = wire->getConnectorWireContext();
                        if (wireContext) {
                            wireContext->setAutoCalculation(true);
                        }
                        wire->linkPortGraphicsObject(newCopyPortGraphicsObject);
                    }
                }
            }
        }
        auto businessHook = canvasContext->getBusinessHooksServer();
        if (businessHook != nullptr) {
            businessHook->copyModelVariableGroups(newSourceProxy, originalSourceProxy);
        }
        canvasScene->deleteTransformProxyGraphicsObject(newProxyGraphicsObject);
        newSourceProxy = newSourceProxyBak;
    }
    startVerifyCanvas();
}
