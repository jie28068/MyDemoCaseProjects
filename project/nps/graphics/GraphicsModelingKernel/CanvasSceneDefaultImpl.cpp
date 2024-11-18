#include "CanvasSceneDefaultImpl.h"
#include "ActionManager.h"
#include "BezierConnectorWireAlgorithm.h"
#include "CanvasSceneDefaultImplPrivate.h"
#include "CanvasViewDefaultImpl.h"
#include "ClipBoardMimeData.h"
#include "ConnectorWireCommand.h"
#include "ConnectorWireContext.h"
#include "ConnectorWireGraphicsObject.h"
#include "ConnectorWireSwitch.h"
#include "CopyRightGraphicsObject.h"
#include "DeleteCommand.h"
#include "GraphicsFactoryManager.h"
#include "GraphicsKernelDefinition.h"
#include "ICanvasGraphicsObjectFactory.h"
#include "ManhattaConnectorWireAlgorithm.h"
#include "NPSPropertyManager.h"
#include "PasteCommand.h"
#include "PortContext.h"
#include "SourceProxy.h"
#include "SourceProxyCommand.h"
#include "StraightLineConnectorWireAlgorithm.h"
#include "SwitchBusbarTypeCommand.h"
#include "TransformCommand.h"
#include "TransformItemGroup.h"
#include "TransformProxyGraphicsObject.h"
#include "TransformProxyOutlineGraphicsObject.h"
#include "Utility.h"
#include "graphicsmodelingkernel.h"

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QDialog>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QScrollBar>
#include <QtConcurrent>
#include <QtCore>

CanvasSceneDefaultImpl::CanvasSceneDefaultImpl(QSharedPointer<CanvasContext> canvasContext, QObject *parent)
    : ICanvasScene(parent)
{
    dataPtr.reset(new CanvasSceneDefaultImplPrivate());
    dataPtr->canvasContext = canvasContext;
    dataPtr->graphicsLayerManager = QSharedPointer<GraphicsLayerManager>(new GraphicsLayerManager(canvasContext));
    dataPtr->promptWire = new ConnectorWirePrompt();
    addItem(dataPtr->promptWire);

    ActionManager &actionMgr = ActionManager::getInstance();

    connect(dataPtr->undoStack.data(), &QUndoStack::canUndoChanged, &actionMgr, &ActionManager::onUndoStatusChanged);
    connect(dataPtr->undoStack.data(), &QUndoStack::canRedoChanged, &actionMgr, &ActionManager::onRedoStatusChanged);

    connect(canvasContext.data(), &CanvasContext::gridFlagChanged, this,
            &CanvasSceneDefaultImpl::onCanvasGridFlagChanged);
    connect(canvasContext.data(), &CanvasContext::refreshCanvas, this, &CanvasSceneDefaultImpl::onRefreshCanvas);

    connect(this, &QGraphicsScene::selectionChanged, this, &CanvasSceneDefaultImpl::onSceneSelectionsChanged);
}

CanvasSceneDefaultImpl::~CanvasSceneDefaultImpl()
{
    freeSource();
    disconnect();
}

void CanvasSceneDefaultImpl::bindSignals() { }

void CanvasSceneDefaultImpl::rotateOnCenter(int angle, Qt::Axis axis)
{
    if (!dataPtr->canvasContext) {
        return;
    }

    setTransformProxyChanging(true);
    QList<QGraphicsItem *> selecteds = selectedItems();

    QMap<QString, PConnectorWireContext> lastConnectorWireCtx;
    QMap<QString, PConnectorWireContext> refreshedConnectorWireCtx;
    for each (auto item in selecteds) {
        if (item->type() == kTransformProxyGraphics) {
            // 保存当前关联的连接线的状态
            TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
            if (proxyGraphics) {
                auto portGraphics = proxyGraphics->getPortGraphicsObjectList();
                for each (auto port in portGraphics) {
                    if (port) {
                        auto wires = port->getLinkedConnectorWireList();
                        for each (auto wire in wires) {
                            PConnectorWireContext wireCtx = wire->getConnectorWireContext();
                            if (!lastConnectorWireCtx.contains(wireCtx->uuid())) {
                                PConnectorWireContext cacheCtx =
                                        QSharedPointer<ConnectorWireContext>(new ConnectorWireContext(*wireCtx));
                                lastConnectorWireCtx[wireCtx->uuid()] = cacheCtx;
                            }
                        }
                    }
                }
            }
        }
    }

    QListIterator<QGraphicsItem *> iter(selecteds);

    TransformCommand *command = new TransformCommand(this);
    while (iter.hasNext()) {
        QGraphicsItem *item = iter.next();
        TransformProxyGraphicsObject *proxy = dynamic_cast<TransformProxyGraphicsObject *>(item);
        if (proxy && proxy->getSourceGraphicsObject()) {
            QSharedPointer<SourceProxy> sourceProxy = proxy->getSourceProxy();
            if (!sourceProxy) {
                continue;
            }
            if (sourceProxy->prototypeName() == TextAnnotationStr)
                continue;
            command->setOldTransforms(proxy->id(), sourceProxy->scaleTransform(), sourceProxy->rotateTransform(),
                                      sourceProxy->translateTransform(), sourceProxy->xAxisFlipTransform(),
                                      sourceProxy->yAxisFlipTransform());
            proxy->rotateOnCenter(angle, axis);
            command->setNewTransforms(proxy->id(), sourceProxy->scaleTransform(), sourceProxy->rotateTransform(),
                                      sourceProxy->translateTransform(), sourceProxy->xAxisFlipTransform(),
                                      sourceProxy->yAxisFlipTransform());
        }
    }

    QMapIterator<QString, PConnectorWireContext> wireCtxIter(lastConnectorWireCtx);
    while (wireCtxIter.hasNext()) {
        wireCtxIter.next();
        QString wireUUID = wireCtxIter.key();
        PConnectorWireContext nowWireCtx = dataPtr->canvasContext->getConnectorWireContext(wireUUID);
        if (!nowWireCtx) {
            continue;
        }
        refreshedConnectorWireCtx[wireUUID] = PConnectorWireContext(new ConnectorWireContext(*nowWireCtx));
    }

    if (refreshedConnectorWireCtx.size() > 0) {
        command->setConnectorWireContext(lastConnectorWireCtx, refreshedConnectorWireCtx);
    }

    if (!command->isEmpty()) {
        dataPtr->undoStack->push(command);
    } else {
        delete command;
    }
    setTransformProxyChanging(false);
}

bool CanvasSceneDefaultImpl::getPasteStatus()
{
    bool bstatus = false;
    if (dataPtr == nullptr || dataPtr->canvasContext == nullptr) {
        return bstatus;
    }
    ClipBoardMimeData *mimeData = dynamic_cast<ClipBoardMimeData *>((QMimeData *)QApplication::clipboard()->mimeData());
    BusinessHooksServer *hookserver = dataPtr->canvasContext->getBusinessHooksServer();
    if (mimeData == nullptr || hookserver == nullptr) {
        return bstatus;
    }
    CanvasContext::Type copytype = mimeData->getCanvasType();
    bstatus = hookserver->enableCreate(copytype, mimeData->getSourcesMap());

    return bstatus;
}

void CanvasSceneDefaultImpl::makeSelectedItemAsGroup(QList<QGraphicsItem *> selecteds)
{
    if (!dataPtr->canvasContext) {
        return;
    }
    auto mode = dataPtr->canvasContext->getInteractionMode();
    if (mode == kAnnotation) {
        return;
    }
    // 注释模块在普通模块上方显示
    qSort(selecteds.begin(), selecteds.end(), [](const QGraphicsItem *item1, const QGraphicsItem *item2) -> bool {
        return item1->zValue() <= item2->zValue();
    });

    if (selecteds.size() > 1) {
        if (dataPtr->itemGroup && dataPtr->itemGroup->isVisible()
            && dataPtr->itemGroup->childItems().size() == selecteds.size()) {
            return;
        }
        processSelectionChanged();

        QList<TransformProxyGraphicsObject *> transformLayers;
        QSet<ConnectorWireGraphicsObject *> connectorLayers;
        for each (auto item in selecteds) {
            GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
            if (layer) {
                if (layer->type() == kTransformProxyGraphics) {
                    auto proxy = dynamic_cast<TransformProxyGraphicsObject *>(layer);
                    if (proxy) {
                        transformLayers.append(proxy);
                        auto linkedWires = proxy->getLinkedConnectorWires();
                        for each (auto wire in linkedWires) {
                            if (wire && !wire->isConnectedWithPort()) {
                                connectorLayers.insert(wire);
                            }
                        }
                    }
                } else if (layer->type() == kConnectorWireGraphics) {
                    auto connector = dynamic_cast<ConnectorWireGraphicsObject *>(layer);
                    if (connector && connector->isConnectedWithPort()) {
                        connectorLayers.insert(connector);
                    }
                } else if (layer->type() == kConnectorWireSegment) {
                    continue;
                } else {
                    // 其他图元取消选中状态,避免只选择资源名称时也可以被拖动
                    layer->setSelected(false);
                }
            }
        }
        if (transformLayers.isEmpty()) {
            return;
        }
        if (transformLayers.size() == 1) {
            for each (auto connector in connectorLayers) {
                if (connector) {
                    connector->setSelected(false);
                }
            }
            return;
        }
        dataPtr->itemGroup = new TransformItemGroup(this);
        for each (auto layer in transformLayers) {
            if (layer) {
                if (layer->parentItem() == dataPtr->itemGroup) {
                    continue;
                }
                layer->addToGroup(dataPtr->itemGroup);
                TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(layer);
                if (proxyGraphics) {
                    proxyGraphics->refreshSizeControlGraphics(false);
                }
            }
        }
        for each (auto connector in connectorLayers) {
            if (connector) {
                if (!connector->isConnectedWithPort()) {
                    connector->addToGroup(dataPtr->itemGroup);
                } else {
                    // 连接线两端连接的资源都是属于选中状态，则把该连接线加入到图元组
                    auto srcPort = connector->getStartPortGraphics();
                    auto dstPort = connector->getEndPortGraphics();
                    if (srcPort && srcPort->group() && dstPort && dstPort->group()) {
                        connector->addToGroup(dataPtr->itemGroup);
                    } else {
                        // 取消选中状态
                        connector->setSelected(false);
                    }
                }
            }
        }

        addItem(dataPtr->itemGroup);
        dataPtr->itemGroup->setSelected(true);
    }
    if (selecteds.isEmpty()) {
        ActionManager::getInstance().getAction(ActionManager::Delete)->setEnabled(false);
    }
}

void CanvasSceneDefaultImpl::autoScroll(QPointF pos)
{
    if (selectedItems().size() > 0) {
        static QDateTime lastTime = QDateTime::currentDateTime();
        auto view = getCanvasView();
        if (view) {
            QRectF viewPortRect = view->viewport()->rect();
            QRectF viewSceneRc = view->mapToScene(viewPortRect.toRect()).boundingRect();

            if (!viewSceneRc.contains(pos)) {
                QDateTime currentTime = QDateTime::currentDateTime();
                qint64 cost_time = lastTime.msecsTo(currentTime);
                if (cost_time >= 25) {
                    lastTime = currentTime;
                    if (pos.y() > viewSceneRc.bottom()) {
                        view->verticalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepAdd);
                    }
                    if (pos.y() < viewSceneRc.top()) {
                        view->verticalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepSub);
                    }
                    if (pos.x() < viewSceneRc.left()) {
                        view->horizontalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepSub);
                    }
                    if (pos.x() > viewSceneRc.right()) {
                        view->horizontalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepAdd);
                    }
                }
                if (!dataPtr->scrollTimer) {
                    dataPtr->scrollTimer = new QTimer();
                    dataPtr->scrollTimer->setInterval(25);
                    connect(dataPtr->scrollTimer, SIGNAL(timeout()), this, SLOT(onScrollTimerOut()));
                    dataPtr->scrollTimer->start();
                }
            } else {
                if (dataPtr->scrollTimer) {
                    dataPtr->scrollTimer->stop();
                    delete dataPtr->scrollTimer;
                    dataPtr->scrollTimer = nullptr;
                }
            }
        }
    } else {
        if (dataPtr->scrollTimer) {
            dataPtr->scrollTimer->stop();
            delete dataPtr->scrollTimer;
            dataPtr->scrollTimer = nullptr;
        }
    }
}

void CanvasSceneDefaultImpl::refreshTransformProxyGraphicsPainterPath()
{
    QPainterPath path;
    auto allProxys = getTransformProxyGraphicsList();
    for each (auto proxy in allProxys) {
        if (proxy) {
            auto source = proxy->getSourceProxy();
            if (source && source->moduleType() != GKD::SOURCE_MODULETYPE_ANNOTATION) {
                QRectF sceneRect = proxy->getTransformSceneRect();
                path.addRect(sceneRect);
            }
        }
    }
    dataPtr->transformProxyGraphicsPath = path;
}

TransformProxyGraphicsObject *CanvasSceneDefaultImpl::getLastClickedPorxyGraphics()
{
    return dataPtr->lastClickedProxyGraphics;
}

void CanvasSceneDefaultImpl::refreshLastClickedProxyGraphics(TransformProxyGraphicsObject *proxyGraphics)
{
    dataPtr->lastClickedProxyGraphics = proxyGraphics;
}

void CanvasSceneDefaultImpl::processSelectionChanged()
{
    QList<QGraphicsItem *> allItems = selectedItems();
    if (allItems.size() <= 1) {
        QMapIterator<QString, ConnectorWireGraphicsObject *> wireIter(dataPtr->connectorGraphicsMap);
        while (wireIter.hasNext()) {
            wireIter.next();
            ConnectorWireGraphicsObject *wire = wireIter.value();
            if (wire) {
                wire->setFlag(QGraphicsItem::ItemIsMovable, false);
            }
        }
    }
    ActionManager &mgr = ActionManager::getInstance();
    // action状态变更
    if (dataPtr == nullptr || dataPtr->canvasContext == nullptr
        || mgr.getActivateCanvasUUID() != dataPtr->canvasContext->uuid()) {
        return;
    }
    if (dataPtr->canvasContext->canModify()) {
        if (allItems.size() > 0) {
            dataPtr->canvasContext->setStatusFlag(CanvasContext::kItemSelectedStatus, true);
            bool copyAble = false;
            bool deleteAble = false;
            bool hasErrorSource = false;
            // 若被选中的模块中含有不可用控制模块
            BusinessHooksServer *hook = dataPtr->canvasContext->getBusinessHooksServer();
            foreach (QGraphicsItem *item, selectedItems()) {
                GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
                if (layer) {
                    if (layer->type() == kTransformProxyGraphics) {
                        copyAble = true;
                        deleteAble = true;
                        if (!hasErrorSource) {
                            TransformProxyGraphicsObject *trans = dynamic_cast<TransformProxyGraphicsObject *>(item);
                            if (trans) {
                                auto sourceProxy = trans->getSourceProxy();
                                if (sourceProxy && sourceProxy->moduleType() != GKD::SOURCE_MODULETYPE_ELECTRICAL
                                    && (sourceProxy->state() == "disable" || sourceProxy->state() == "error")) {
                                    if (hook && hook->isBlockCtrlUserCombined(sourceProxy)) {
                                        hasErrorSource = true;
                                    } else {
                                        hasErrorSource = false;
                                    }
                                }
                            }
                        }

                    } else if (layer->type() == kConnectorWireSegment || layer->type() == kConnectorWireGraphics) {
                        // 连接线或者连接线线段
                        deleteAble = true;
                    } else {
                    }
                }
            }
            if (hasErrorSource) {
                copyAble = false;
            }
            dataPtr->canvasContext->setStatusFlag(CanvasContext::kItemSelectedStatus, copyAble);
            dataPtr->canvasContext->setStatusFlag(CanvasContext::kItemCanDeleteStatus, deleteAble);
        } else {
            dataPtr->canvasContext->setStatusFlag(CanvasContext::kItemSelectedStatus, false);
        }
    }
}

void CanvasSceneDefaultImpl::processAction(ActionManager::ActionType type)
{
    if (!dataPtr->canvasContext) {
        return;
    }

    switch (type) {
    case ActionManager::SelectAll: {
        // 全选
        auto allItems = items();
        if (allItems.size() <= 0) {
            return;
        }
        if (dataPtr->itemGroup) {
            dataPtr->itemGroup->clear();
        }

        QPainterPath path;
        path.addRect(sceneRect());
        setSelectionArea(path);
        makeSelectedItemAsGroup(allItems);
    } break;
    case ActionManager::Copy: {
        // 拷贝,把资源和连接线信息拷贝到剪切板
        if (selectedItems().size() > 0) {
            CanvasContext::Type type = dataPtr->canvasContext->type();
            ClipBoardMimeData *mimeData = new ClipBoardMimeData(type);
            QListIterator<QGraphicsItem *> iter(selectedItems());
            while (iter.hasNext()) {
                QGraphicsItem *item = iter.next();
                GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
                if (layer) {
                    mimeData->addGraphicsLayer(layer);
                }
            }
            BusinessHooksServer *hook = dataPtr->canvasContext->getBusinessHooksServer();
            // 放入画板uuid @LY
            QString canvanUUID;
            if (hook) {
                canvanUUID = hook->getTopParentModelUUID();
            }
            QByteArray data;
            QDataStream dataStream(&data, QIODevice::WriteOnly);
            dataStream << canvanUUID;
            mimeData->setData("MIMETYPE_CANVAS_UUID", data);

            // 放入剪切操作标志 @WYZ
            bool isCutOperation = false;
            QByteArray dataCutOperationFlag;
            QDataStream dataCutFlagStream(&dataCutOperationFlag, QIODevice::WriteOnly);
            dataCutFlagStream << isCutOperation;
            mimeData->setData("IS_CUT_OPERATION", dataCutOperationFlag);

            // 放入资源所属画板所在项目标识 @TXY
            if (hook) {
                QString projectSign = hook->getCurProjectSign();
                QByteArray data;
                QDataStream dataStream(&data, QIODevice::WriteOnly);
                dataStream << projectSign;
                mimeData->setData("MIMETYPE_PROJECT_SIGN", data);
            }

            QApplication::clipboard()->setMimeData(mimeData);
        }
    } break;
    case ActionManager::Cut: {
        if (selectedItems().size() > 0) {
            DeleteCommand *deleteCommand = new DeleteCommand(this);
            CanvasContext::Type type = dataPtr->canvasContext->type();
            ClipBoardMimeData *mimeData = new ClipBoardMimeData(type);
            QListIterator<QGraphicsItem *> iter(selectedItems());
            while (iter.hasNext()) {
                QGraphicsItem *item = iter.next();
                GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
                if (layer) {
                    mimeData->addGraphicsLayer(layer);
                    deleteCommand->addDeleteGraphics(layer);
                }
            }
            // 放入画板uuid @LY
            QString canvanUUID = dataPtr->canvasContext->uuid();
            QByteArray data;
            QDataStream dataStream(&data, QIODevice::WriteOnly);
            dataStream << canvanUUID;
            mimeData->setData("MIMETYPE_CANVAS_UUID", data);

            // 放入剪切操作标志 @WYZ
            bool isCutOperation = true;
            QByteArray dataCutOperationFlag;
            QDataStream dataCutFlagStream(&dataCutOperationFlag, QIODevice::WriteOnly);
            dataCutFlagStream << isCutOperation;
            mimeData->setData("IS_CUT_OPERATION", dataCutOperationFlag);

            if (dataPtr->itemGroup) {
                dataPtr->itemGroup->clear();
            }

            QApplication::clipboard()->setMimeData(mimeData);
            if (dataPtr->undoStack) {
                dataPtr->undoStack->push(deleteCommand);
            }
            // 剪切之后，被选择的项就删除了
            dataPtr->canvasContext->setStatusFlag(CanvasContext::kItemSelectedStatus, false);
        }
    } break;
    case ActionManager::Paste: {
        // 粘贴
        ClipBoardMimeData *mimeData =
                dynamic_cast<ClipBoardMimeData *>((QMimeData *)QApplication::clipboard()->mimeData());
        if (mimeData) {
            bool isCutOperation = false;
            QByteArray data = QApplication::clipboard()->mimeData()->data("IS_CUT_OPERATION");
            QDataStream dataStream(&data, QIODevice::ReadOnly);
            dataStream >> isCutOperation;

            PasteCommand *command = new PasteCommand(this);
            command->setCutFlag(isCutOperation);
            command->loadClipboardMimeData(mimeData);
            if (dataPtr->undoStack) {
                dataPtr->undoStack->push(command);
            }

            if (isCutOperation) {

                isCutOperation = false;
                QByteArray dataCutOperationFlag;
                QDataStream dataCutFlagStream(&dataCutOperationFlag, QIODevice::WriteOnly);
                dataCutFlagStream << isCutOperation;
                mimeData->setData("IS_CUT_OPERATION", dataCutOperationFlag);

                QApplication::clipboard()->clear();
            }
        }
    } break;
    case ActionManager::Delete: {
        auto selecteds = selectedItems();
        // 删除图元
        if (selecteds.size() > 0) {
            DeleteCommand *command = new DeleteCommand(this);
            QListIterator<QGraphicsItem *> iter(selecteds);
            while (iter.hasNext()) {
                QGraphicsItem *item = iter.next();
                GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
                if (layer) {
                    command->addDeleteGraphics(layer);
                }
            }
            if (dataPtr->itemGroup) {
                dataPtr->itemGroup->clear();
            }
            if (dataPtr->undoStack) {
                dataPtr->undoStack->push(command);
            }
        }
    } break;
    case ActionManager::RotateClockwise: {
        // 顺时针旋转90度
        rotateOnCenter(90);
    } break;
    case ActionManager::RotateAntiClockwise: {
        // 逆时针旋转90度
        rotateOnCenter(-90);
    } break;
    case ActionManager::Rotate180: {
        // 逆时针旋转90度
        rotateOnCenter(180);
    } break;
    case ActionManager::FlipHorizontal: {
        // 水平翻转
        rotateOnCenter(180, Qt::YAxis);
    } break;
    case ActionManager::FlipVertical: {
        rotateOnCenter(180, Qt::XAxis);
    } break;

    case ActionManager::Undo: {
        dataPtr->undoStack->undo();
    } break;
    case ActionManager::Redo: {
        dataPtr->undoStack->redo();
    } break;
    case ActionManager::Prints:
    case ActionManager::PrintDrawBoard: {
        // 打印画板
        print();
    } break;
    case ActionManager::ShowCoverage: {
        // 显示图例
        // actionShowCoverage();
    } break;
    case ActionManager::BreakLinkLine: {
        onBreakLinkLine();
    } break;
    case ActionManager::updateModule: {
        onUpdateModule();
    } break;
    case ActionManager::PecadCase: {
        BusinessHooksServer *hookserver = dataPtr->canvasContext->getBusinessHooksServer();
        if (!hookserver)
            return;
        hookserver->generatePecadCase();
    } break;
    case ActionManager::SwitchBusbarType: {
        QList<QGraphicsItem *> selecteds = selectedItems();
        for each (auto item in selecteds) {
            if (item->type() == kTransformProxyGraphics) {
                TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
                if (proxyGraphics) {
                    auto sourceProxy = proxyGraphics->getSourceProxy();
                    if (sourceProxy) {
                        QString prototypeName = sourceProxy->prototypeName();
                        if (prototypeName == "Busbar" || prototypeName == "DotBusbar") {
                            SwitchBusbarTypeCommand *command = new SwitchBusbarTypeCommand(this);
                            command->setSourcePorxy(sourceProxy);
                            if (dataPtr->undoStack) {
                                dataPtr->undoStack->push(command);
                            }
                        }
                    }
                }
            }
        }
    } break;

    case ActionManager::SetBackgroundColor:
        setBrackgroundImage(BrackGound::brackgoundColor);
        break;
    case ActionManager::ResetBackgroundColor: {
        QList<QGraphicsItem *> select = selectedItems();
        for (auto item : select) {
            TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
            if (proxyGraphics) {
                auto sourceProxy = proxyGraphics->getSourceProxy();
                if (sourceProxy) {
                    sourceProxy->getSourceProperty().setBackgroundColor(QColor(255, 255, 255, 255));
                }
            }
        }
    } break;
    case ActionManager::SetBackgroundImage:
        setBrackgroundImage(BrackGound::brackgoundImage);
        break;
    case ActionManager::ResetBackgroundImage: {
        QList<QGraphicsItem *> select = selectedItems();
        for (auto item : select) {
            TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
            if (proxyGraphics) {
                auto sourceProxy = proxyGraphics->getSourceProxy();
                if (sourceProxy) {
                    sourceProxy->getSourceProperty().setBackgroundImage(QByteArray(), true);
                }
            }
        }

    } break;
    case ActionManager::SetForegroundImage:
        setBrackgroundImage(BrackGound::foregoundImage);
        break;
    case ActionManager::ResetForegroundImage: {
        QList<QGraphicsItem *> select = selectedItems();
        for (auto item : select) {
            TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
            if (proxyGraphics) {
                auto sourceProxy = proxyGraphics->getSourceProxy();
                if (sourceProxy) {
                    sourceProxy->getSourceProperty().setForegroundImage(QByteArray());
                }
            }
        }

    } break;
    case ActionManager::Help: {
        QList<QGraphicsItem *> select = selectedItems();
        for (auto item : select) {
            TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
            if (proxyGraphics) {
                auto sourceProxy = proxyGraphics->getSourceProxy();
                if (sourceProxy) {
                    QString prototypeName = sourceProxy->prototypeName();
                    BusinessHooksServer *hookserver = dataPtr->canvasContext->getBusinessHooksServer();
                    if (!hookserver)
                        return;
                    hookserver->openShowHelpWidget(prototypeName);
                }
            }
        }
    } break;
    case ActionManager::MarkThrough: {
        QList<QGraphicsItem *> selecteds = selectedItems();
        bool needMessageBox = false;
        for each (auto item in selecteds) {
            if (item->type() == kTransformProxyGraphics) {
                TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
                if (!proxyGraphics)
                    continue;
                auto sourceProxy = proxyGraphics->getSourceProxy();
                if (!sourceProxy)
                    continue;
                QString prototypeName = sourceProxy->prototypeName();
                if (prototypeName == "In" || prototypeName == "Out") { // in out模块不设置注释
                    needMessageBox = true;
                    continue;
                }
                int inputCounts = 0;
                int outputCounts = 0;
                for each (auto port in sourceProxy->portList()) {
                    if (port->type() == "input") {
                        ++inputCounts;
                    } else {
                        ++outputCounts;
                    }
                }
                if (inputCounts == outputCounts) {
                    sourceProxy->setCommentState("through");
                } else {
                    needMessageBox = true;
                }
            }
        }
        if (needMessageBox) {
            KMessageBox::information(tr("Inconsistent number of input and output ports"));
        }

    } break;
    case ActionManager::MarkDisable: {
        QList<QGraphicsItem *> selecteds = selectedItems();
        for each (auto item in selecteds) {
            if (item->type() == kTransformProxyGraphics) {
                TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
                if (proxyGraphics) {
                    auto sourceProxy = proxyGraphics->getSourceProxy();
                    if (sourceProxy) {
                        QString protoType = sourceProxy->prototypeName();
                        if (protoType != "In" && protoType != "Out") {
                            sourceProxy->setCommentState("disable");
                            sourceProxy->setBadgeMsg("");
                        }
                    }
                }
            }
        }

    } break;
    case ActionManager::Unmark: {
        QList<QGraphicsItem *> selecteds = selectedItems();
        for each (auto item in selecteds) {
            if (item->type() == kTransformProxyGraphics) {
                TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
                if (proxyGraphics) {
                    auto sourceProxy = proxyGraphics->getSourceProxy();
                    if (sourceProxy) {
                        sourceProxy->setCommentState("normal");
                    }
                }
            }
        }

    } break;
    default:
        break;
    }
}

void CanvasSceneDefaultImpl::updateActionStatus()
{
    if (dataPtr->selectedTransformProxys.isEmpty()) {
        // 没有代理图层被选中
        dataPtr->canvasContext->setStatusFlag(CanvasContext::kItemSelectedStatus, false);
    } else {
        dataPtr->canvasContext->setStatusFlag(CanvasContext::kItemSelectedStatus, true);
        dataPtr->canvasContext->setStatusFlag(CanvasContext::kItemCanDeleteStatus, true);
        // 当资源被创建时，canvascontext的信号被外部插件给block了，所以信号不会触发回调函数
        bool enableTransform = false;
        if (dataPtr->canvasContext->canModify()) {
            ActionManager &actionMgr = ActionManager::getInstance();
            actionMgr.getCopyAbleActionGroup()->setEnabled(true);
            actionMgr.getTransformActionGroup()->setEnabled(true);
            actionMgr.getAction(ActionManager::Delete)->setEnabled(true);
            // 文本注解没有旋转选项
            for each (auto item in dataPtr->selectedTransformProxys) {
                if (item && item->getSourceProxy()) {
                    auto sourceProxy = item->getSourceProxy();
                    if (sourceProxy->moduleType() == GKD::SOURCE_MODULETYPE_ANNOTATION) {
                        if (sourceProxy->prototypeName() != TextAnnotationStr) {
                            enableTransform = true;
                            break;
                        }

                    } else {
                        enableTransform = true;
                        break;
                    }
                }
            }
        } else {
            ActionManager &actionMgr = ActionManager::getInstance();
            actionMgr.getAction(ActionManager::Delete)->setEnabled(false);
        }

        auto transformActionGroup = ActionManager::getInstance().getTransformActionGroup();
        if (transformActionGroup) {
            bool flag = transformActionGroup->isEnabled();
            if (flag != enableTransform) {
                transformActionGroup->setEnabled(enableTransform);
            }
        }
    }
}

void CanvasSceneDefaultImpl::setBrackgroundImage(BrackGound gound)
{
    QList<QGraphicsItem *> select = selectedItems();
    for (auto item : select) {
        TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(item);
        if (proxyGraphics) {
            auto sourceProxy = proxyGraphics->getSourceProxy();
            if (sourceProxy) {
                if (BrackGound::brackgoundColor == gound) {
                    KColorDialog dlg(tr("Select Color"));
                    dlg.setCurrentColor(sourceProxy->getSourceProperty().getBackgroundColor());
                    if (dlg.exec() == KColorDialog::Ok) {
                        sourceProxy->getSourceProperty().setBackgroundColor(dlg.currentColor());
                    }
                } else {
                    QString filePath = QFileDialog::getOpenFileName(&QWidget(), tr("Select Image"), "",
                                                                    tr("Image Files (*.png *.jpg *.bmp)"));
                    if (!filePath.isEmpty()) {
                        QFile file(filePath);
                        if (file.open(QIODevice::ReadOnly)) {
                            QByteArray byteArray = file.readAll();
                            if (gound == BrackGound::brackgoundImage) {
                                sourceProxy->getSourceProperty().setBackgroundImage(byteArray, true);
                            } else {
                                sourceProxy->getSourceProperty().setForegroundImage(byteArray);
                            }
                        }
                        file.close();
                    }
                }
            }
        }
    }
}

bool CanvasSceneDefaultImpl::print()
{
    if (dataPtr == nullptr || dataPtr->canvasContext == nullptr) {
        return false;
    }
    QPrinter printer(QPrinter::HighResolution);
    printer.setOrientation(QPrinter::Landscape);

    QSize drawingBoardSize = dataPtr->canvasContext->size();
    if (drawingBoardSize == QSize(1189, 841)) {
        printer.setPageSize(QPrinter::A0);
    } else if (drawingBoardSize == QSize(841, 594)) {
        printer.setPageSize(QPrinter::A1);
    } else if (drawingBoardSize == QSize(594, 420)) {
        printer.setPageSize(QPrinter::A2);
    } else if (drawingBoardSize == QSize(420, 297)) {
        printer.setPageSize(QPrinter::A3);
    } else if (drawingBoardSize == QSize(297, 210)) {
        printer.setPageSize(QPrinter::A4);
    }

    /*画板打印*/
    QPrintDialog printDialog(&printer, getCanvasView());
    if (printDialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        painter.setRenderHint(QPainter::Antialiasing);
        dataPtr->canvasContext->setGridFlag(CanvasContext::kGridNoneFlag);
        render(&painter); //, QRectF(), itemsBoundingRect());
        dataPtr->canvasContext->setGridFlag(CanvasContext::kGridShowFlag);
    }
    return true;
}

void CanvasSceneDefaultImpl::initCopyRight()
{
    if (!dataPtr->copyRightGraphicsObject) {
        CopyRightGraphicsObject *copyRight = new CopyRightGraphicsObject(this, nullptr);
        dataPtr->copyRightGraphicsObject = copyRight;
        connect(this, SIGNAL(sceneRectChanged(const QRectF &)), this, SLOT(onSceneRectChanged(const QRectF &)));
        addItem(copyRight);
        onSceneRectChanged(sceneRect());
        QRectF rect = sceneRect();
        QRectF copyRightRc = dataPtr->copyRightGraphicsObject->boundingRect();
        QPointF pos = QPointF(rect.right() - copyRightRc.width() - 10,
                              rect.bottom() - copyRightRc.height() - 10); // 减10为了图例与scene边界有间隔
        pos = Utility::pointAlignmentToGrid(pos, getGridSpace());
        dataPtr->copyRightGraphicsObject->setPos(pos);
    }
}

TransformProxyGraphicsObject *CanvasSceneDefaultImpl::addSourceToScene(QSharedPointer<SourceProxy> source,
                                                                       bool needUodo)
{
    if (!source) {
        return nullptr;
    }
    QString moduleType = source->moduleType();
    QSharedPointer<ICanvasGraphicsObjectFactory> factory = GraphicsFactoryManager::getInstance().getFactory(moduleType);
    if (factory.isNull()) {
        return nullptr;
    }
    TransformProxyGraphicsObject *transformProxyGraphics = factory->createTransformProxyGraphicsObject(this, source);
    if (!transformProxyGraphics) {
        return nullptr;
    }
    addTransformProxyGraphicsObject(transformProxyGraphics);

    if (needUodo) {
        QString prototypeName = source->prototypeName();
        AddSourceProxyCommand *command = new AddSourceProxyCommand(this);
        command->addSourceProxy(source);
        if (dataPtr->undoStack) {
            dataPtr->undoStack->push(command);
        }
    }

    return transformProxyGraphics;
}

bool CanvasSceneDefaultImpl::deleteSourceFromScene(QSharedPointer<SourceProxy> source)
{
    if (!source) {
        return false;
    }
    TransformProxyGraphicsObject *trans = getTransformProxyGraphicsByID(source->uuid());
    if (trans) {
        deleteTransformProxyGraphicsObject(trans);
        return true;
    }

    return false;
}

bool CanvasSceneDefaultImpl::loadFromSourceProxy(QSharedPointer<SourceProxy> source)
{
    if (!source) {
        return false;
    }
    QString moduleType = source->moduleType();
    QSharedPointer<ICanvasGraphicsObjectFactory> factory = GraphicsFactoryManager::getInstance().getFactory(moduleType);
    if (factory.isNull()) {
        return false;
    }
    if (moduleType == "annotation") {
        if (source->size().width() <= 0 || source->size().height() <= 0) {
            return false;
        }
    }
    setLoadingCanvasStatus(true);
    TransformProxyGraphicsObject *transformProxyGraphics = factory->createTransformProxyGraphicsObject(this, source);
    if (!transformProxyGraphics) {
        setLoadingCanvasStatus(false);
        return false;
    }
    addTransformProxyGraphicsObject(transformProxyGraphics);
    setLoadingCanvasStatus(false);
    return true;
}

QSharedPointer<CanvasContext> CanvasSceneDefaultImpl::getCanvasContext()
{
    return dataPtr->canvasContext;
}

void CanvasSceneDefaultImpl::addTransformProxyGraphicsObject(TransformProxyGraphicsObject *transformProxyGraphics)
{
    if (nullptr == transformProxyGraphics || !dataPtr->canvasContext) {
        return;
    }
    QString proxyID = transformProxyGraphics->id();
    if (dataPtr->proxyGraphicsMap.contains(proxyID)) {
        return;
    }
    dataPtr->proxyGraphicsMap.insert(proxyID, transformProxyGraphics);
    dataPtr->canvasContext->addSourceProxy(transformProxyGraphics->getSourceProxy());
    addItem(transformProxyGraphics);

    connect(transformProxyGraphics, SIGNAL(transformChanged(QRectF, int)), this,
            SLOT(onTransformProxyTransformChanged(QRectF, int)));
    connect(transformProxyGraphics, &TransformProxyGraphicsObject::selectedChanged, this,
            &CanvasSceneDefaultImpl::onTransformProxySelectedChanged);
    connect(transformProxyGraphics, SIGNAL(deleteSource(QString)), this, SLOT(onDeleteSource(QString)));

    if (transformProxyGraphics->isSelected()) {
        dataPtr->selectedTransformProxys.push_back(transformProxyGraphics);
        updateActionStatus();
    }

    refreshTransformProxyGraphicsPainterPath();
}

void CanvasSceneDefaultImpl::deleteTransformProxyGraphicsObject(TransformProxyGraphicsObject *transformGraphics)
{
    if (nullptr == transformGraphics) {
        return;
    }

    // 把图元从场景中删除
    QString id = transformGraphics->id();
    dataPtr->proxyGraphicsMap.remove(id);

    PSourceProxy sourceProxy = transformGraphics->getSourceProxy();
    if (sourceProxy && dataPtr->canvasContext) {
        // 删除画板上下文资源映射表中的资源
        QString sourceUUID = sourceProxy->uuid();
        dataPtr->canvasContext->deleteSourceProxy(sourceUUID);
    }
    auto lastClickProxyGraphcs = getLastClickedPorxyGraphics();
    if (lastClickProxyGraphcs == transformGraphics) {
        refreshLastClickedProxyGraphics(nullptr);
    }

    dataPtr->selectedTransformProxys.removeAll(transformGraphics);

    transformGraphics->cleanLayerInfo();
    removeItem(transformGraphics);

    refreshTransformProxyGraphicsPainterPath();

    // add by liwenyu  2023.09.22
    // 此处处理是防止在创建注解图元的过程中，注解图元被删除(比如折线在没有创建完成时，按ctrl+z撤销)
    // 需要重置view里面的注解对象，否则会导致访问野指针崩溃
    if (sourceProxy->moduleType() == GKD::SOURCE_MODULETYPE_ANNOTATION) {
        // 如果当前资源是注解
        CanvasViewDefaultImpl *defaultView = dynamic_cast<CanvasViewDefaultImpl *>(dataPtr->canvasView);
        if (defaultView) {
            defaultView->resetAnnotationGraphicsObject(transformGraphics->getSourceGraphicsObject());
        }
    }
    // end

    transformGraphics->deleteLater();
}

void CanvasSceneDefaultImpl::addConnectorWireGraphicsObject(ConnectorWireGraphicsObject *connectorWireGraphics,
                                                            bool canUndo)
{
    if (nullptr == connectorWireGraphics) {
        return;
    }

    QSharedPointer<ConnectorWireContext> connectorContext = connectorWireGraphics->getConnectorWireContext();
    QString uuid = connectorContext->uuid();
    if (!dataPtr->connectorGraphicsMap.contains(uuid)) {
        dataPtr->connectorGraphicsMap[uuid] = connectorWireGraphics;
        addItem(connectorWireGraphics);
    }

    if (canUndo) {
        AddConnectorWireCommand *command = new AddConnectorWireCommand(this);
        command->addConnectorWireContext(connectorContext);
        if (dataPtr->undoStack) {
            dataPtr->undoStack->push(command);
        }
        connectorWireGraphics->onTransformProxyChanged();
    }
    // 获取新连接线的输出端口，然后计算该输出的连接线的分支点
    PortGraphicsObject *outputPort = connectorWireGraphics->getOutputTypePortGraphics();
    if (outputPort) {
        outputPort->calcuteConnectorWireBranchPoints();
    }
}

void CanvasSceneDefaultImpl::deleteConnectorWireGraphicsObject(ConnectorWireGraphicsObject *connectorWireGraphics)
{
    if (nullptr == connectorWireGraphics || !dataPtr->canvasContext) {
        return;
    }
    qDebug() << "删除连接线图元:" << connectorWireGraphics << ",ID:" << connectorWireGraphics->id();

    QSharedPointer<ConnectorWireContext> connectorContext = connectorWireGraphics->getConnectorWireContext();
    QString uuid = connectorContext->uuid();

    // 被删除的连接线所连接的输出端口
    PortGraphicsObject *outputPort = connectorWireGraphics->getOutputTypePortGraphics();

    // 清理连接线所连接的端口关联数据
    connectorWireGraphics->clearLinks();

    // 重新计算分支点
    if (outputPort) {
        outputPort->calcuteConnectorWireBranchPoints();
    }

    // 缓存的连接线映射表删除该连接线
    dataPtr->connectorGraphicsMap.remove(uuid);
    // 场景中删除连接线图元
    connectorWireGraphics->cleanLayerInfo();
    removeItem(connectorWireGraphics);
    connectorWireGraphics->deleteLater();

    // 删除连接线的同时，需要把相关的数据删除
    dataPtr->canvasContext->deleteConnectWireContext(uuid);
}

void CanvasSceneDefaultImpl::deleteAllConnectorWireGraphicsObject()
{
    auto connectorList = dataPtr->connectorGraphicsMap.values();
    if (connectorList.isEmpty())
        return;
    DeleteCommand *command = new DeleteCommand(this);
    foreach (auto connector, connectorList) {
        GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(connector);
        if (layer)
            command->addDeleteGraphics(layer);
    }
    if (dataPtr->undoStack) {
        dataPtr->undoStack->push(command);
    }
}

void CanvasSceneDefaultImpl::deleteConnectorWireGraphicsObjectById(QString uuid)
{
    TransformProxyGraphicsObject *transGraphic = this->getTransformProxyGraphicsByID(uuid);
    if (transGraphic) {
        QList<PortGraphicsObject *> portGraphicsList = transGraphic->getPortGraphicsObjectList();
        // 遍历端口,获取端口的连接线
        QListIterator<PortGraphicsObject *> iterPort(portGraphicsList);
        while (iterPort.hasNext()) {
            PortGraphicsObject *portGraphics = iterPort.next();
            if (nullptr != portGraphics) {
                // 端口连接线列表
                QList<ConnectorWireGraphicsObject *> connectorWireList = portGraphics->getLinkedConnectorWireList();
                QListIterator<ConnectorWireGraphicsObject *> iterConnector(connectorWireList);
                while (iterConnector.hasNext()) {
                    ConnectorWireGraphicsObject *connectorWireGraphics = iterConnector.next();
                    if (connectorWireGraphics) {
                        deleteConnectorWireGraphicsObject(connectorWireGraphics);
                    }
                }
            }
        }
    }
}

ConnectorWireGraphicsObject *CanvasSceneDefaultImpl::getConnectorWireGraphicsByID(QString id)
{
    return dataPtr->connectorGraphicsMap.value(id, nullptr);
}

TransformProxyGraphicsObject *CanvasSceneDefaultImpl::getTransformProxyGraphicsByID(QString id)
{
    if (!id.startsWith(ProxyIDPrefix)) {
        id = ProxyIDPrefix + id;
    }
    return dataPtr->proxyGraphicsMap.value(id, nullptr);
}

QList<TransformProxyGraphicsObject *> CanvasSceneDefaultImpl::getTransformProxyGraphicsList()
{
    return dataPtr->proxyGraphicsMap.values();
}

QList<TransformProxyGraphicsObject *> CanvasSceneDefaultImpl::getSelectedTransformProxyGraphicsList()
{
    return QList<TransformProxyGraphicsObject *>();
}

TransformProxyGraphicsObject *
CanvasSceneDefaultImpl::getTransformProxyGraphicsBySource(QSharedPointer<SourceProxy> source)
{
    if (source.isNull()) {
        return nullptr;
    }
    return getTransformProxyGraphicsByID(source->uuid());
}

QSharedPointer<CustomUndoStack> CanvasSceneDefaultImpl::getUndoStack()
{
    return dataPtr->undoStack;
}

QSharedPointer<GraphicsLayerManager> CanvasSceneDefaultImpl::getGraphicsLayerManager()
{
    return dataPtr->graphicsLayerManager;
}

bool CanvasSceneDefaultImpl::allowOverlap()
{
    return dataPtr->canvasContext->getAllowOverlap();
}

PortGraphicsObject *CanvasSceneDefaultImpl::getPortGraphics(PPortContext ctx)
{
    if (!ctx) {
        return nullptr;
    }
    QString sourceID = ctx->sourceUUID();
    QString portID = ctx->uuid();
    return getPortGraphics(sourceID, portID);
}

PortGraphicsObject *CanvasSceneDefaultImpl::getPortGraphics(const QString &sourceID, const QString &portID)
{
    TransformProxyGraphicsObject *proxyGraphics = this->getTransformProxyGraphicsByID(sourceID);
    if (!proxyGraphics) {
        return nullptr;
    }
    return proxyGraphics->getPortGraphicsObject(portID);
}

bool CanvasSceneDefaultImpl::isLoadingCanvas()
{
    return dataPtr->isLoadingCanvas;
}

void CanvasSceneDefaultImpl::setLoadingCanvasStatus(bool status)
{
    dataPtr->isLoadingCanvas = status;
}

void CanvasSceneDefaultImpl::onBackgroundColorChanged(QColor color) { }

void CanvasSceneDefaultImpl::onSizeChanged(QSize size) { }

void CanvasSceneDefaultImpl::onScaleChanged(int scale) { }

void CanvasSceneDefaultImpl::drawBackground(QPainter *painter, const QRectF &rect)
{
    QSharedPointer<CanvasContext> ctx = dataPtr->canvasContext;
    if (ctx.isNull()) {
        return;
    }

    painter->save();

    QColor backgroundColor = ctx->backgroundColor();
    QRectF rectScene = sceneRect();
    // 非sceneRect区域显示在view中时，先填充一个固定底色（如缩放到最小会显示边缘）
    if (!rectScene.contains(rect)) {
        // 填充固定颜色
        painter->fillRect(rect, QColor(250, 250, 250));
    }
    rectScene.setRect(rectScene.x(), rectScene.y(), rectScene.width() + 20, rectScene.height() + 20);
    painter->fillRect(rectScene, backgroundColor);

    QPen pen(Qt::darkCyan);
    // QPen pen(Qt::black);
    pen.setStyle(Qt::DotLine);
    pen.setWidthF(0.2);
    painter->setPen(pen);
    painter->setRenderHints(QPainter::Antialiasing, false);

    bool showGrid = (CanvasContext::kGridShowFlag == (ctx->gridFlag() & CanvasContext::kGridShowFlag));
    bool showDotGrid = (CanvasContext::kGridShowDotGrid == (ctx->gridFlag() & CanvasContext::kGridShowDotGrid));
    if (!ctx->canModify()) {
        // 画板不可编辑的情况下，不显示网格线
        showGrid = false;
        showDotGrid = false;
    }
    // 绘制网格线，画板打印时不绘制
    if (showGrid) {
        painter->drawLines(dataPtr->backgroundLines);
    } else if (showDotGrid) {
        pen.setWidthF(0.4);
        painter->setPen(pen);
        painter->drawPoints(dataPtr->backgroundPoints);
    }

    painter->restore();
}

QPointF CanvasSceneDefaultImpl::getMouseClickedPos()
{
    return dataPtr->mouseClickedPos;
}

TransformItemGroup *CanvasSceneDefaultImpl::getTransformItemGroup()
{
    return dataPtr->itemGroup;
}

void CanvasSceneDefaultImpl::setTransformItemGroup(TransformItemGroup *group)
{
    dataPtr->itemGroup = group;
}

void CanvasSceneDefaultImpl::setCanvasView(ICanvasView *view)
{
    dataPtr->canvasView = view;
}

ICanvasView *CanvasSceneDefaultImpl::getCanvasView()
{
    return dataPtr->canvasView;
}

QPainterPath CanvasSceneDefaultImpl::getTransformProxyGraphicsPainterPath()
{
    return dataPtr->transformProxyGraphicsPath;
}

QList<QGraphicsItem *> CanvasSceneDefaultImpl::selectedItems() const
{
    if (dataPtr->itemGroup && dataPtr->itemGroup->childItems().size() > 0) {
        return dataPtr->itemGroup->childItems();
    } else {
        return ICanvasScene::selectedItems();
    }
}

void CanvasSceneDefaultImpl::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (!dataPtr->canvasContext) {
        return;
    }
    auto mode = dataPtr->canvasContext->getInteractionMode();
    if (mode == kAnnotation || mode == kAreaAmplification || mode == kHandDrag) {
        // 设置为正常状态
        dataPtr->canvasContext->setInteractionMode(kNormal);
        return;
    }
    QGraphicsScene::contextMenuEvent(event);
    if (event->isAccepted()) {
        return;
    }

    QSharedPointer<QMenu> pMenu(new QMenu);
    if (!dataPtr->canvasContext)
        return;

    ActionManager::getInstance().sceneMenu(pMenu.data(), dataPtr->canvasContext);

    pMenu->setObjectName("uniformStyleMenu");
    pMenu->exec(event->screenPos());
    event->accept();
}

void CanvasSceneDefaultImpl::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{

    setMouseClickedPort(nullptr);
    if (mouseEvent->button() == Qt::LeftButton) {
        dataPtr->isMouseLeftPressed = true;
    }
    QPointF scenePos = mouseEvent->scenePos();

    if (!itemAt(scenePos)) {
        // 如果鼠标点击的坐标下没有图元，则更新鼠标点击坐标
        dataPtr->mouseClickedPos = scenePos;
        if (dataPtr->subsystemAreaGraphics && dataPtr->subsystemAreaGraphics->isVisible()) {
            QRectF susSystemSceneRect = dataPtr->subsystemAreaGraphics->boundingRect();
            if (!susSystemSceneRect.contains(scenePos)) {
                dataPtr->subsystemAreaGraphics->setVisible(false);
            }
        }
        dataPtr->canvasContext->setSubSystemClickPos(scenePos);
    } else {
        if (dataPtr->subsystemAreaGraphics) {
            if (dataPtr->subsystemAreaGraphics->isVisible()) {
                QRectF susSystemSceneRect = dataPtr->subsystemAreaGraphics->boundingRect();
                if (susSystemSceneRect.contains(scenePos)) {
                    if (!selectedItems().isEmpty()) {
                        dataPtr->subsystemAreaGraphics->setVisible(false);
                    }
                }
            }
        }
        dataPtr->canvasContext->setSubSystemClickPos(QPointF());
    }

    ICanvasScene::mousePressEvent(mouseEvent);
}

void CanvasSceneDefaultImpl::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QPointF pt = mouseEvent->scenePos();
    if (dataPtr->subsystemAreaGraphics && dataPtr->subsystemAreaGraphics->isVisible()) {
        QRectF detectRect = dataPtr->subsystemAreaGraphics->boundingRect().adjusted(-100, -100, 100, 100);
        if (!detectRect.contains(pt)) {
            dataPtr->subsystemAreaGraphics->setVisible(false);
        }
    }
    autoScroll(pt);
    ICanvasScene::mouseMoveEvent(mouseEvent);
}

void CanvasSceneDefaultImpl::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        dataPtr->isMouseLeftPressed = false;
        if (dataPtr->positionCommand) {
            dataPtr->undoStack->push(dataPtr->positionCommand);
            dataPtr->positionCommand = nullptr;
        }
        if (dataPtr->sourceNamePositionCommand) {
            dataPtr->undoStack->push(dataPtr->sourceNamePositionCommand);
            dataPtr->sourceNamePositionCommand = nullptr;
        }
    }
    QList<QGraphicsItem *> selecteds = selectedItems();
    bool hasCopyingSource = false;
    bool hasProxyGraphics = false;
    for each (auto item in selecteds) {
        GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
        if (layer) {
            if (layer->type() == kTransformProxyGraphics) {
                TransformProxyGraphicsObject *proxyGraphics = dynamic_cast<TransformProxyGraphicsObject *>(layer);
                proxyGraphics->clearAlignLine();
                if (proxyGraphics->isCopying()) {
                    hasCopyingSource = true;
                }
                hasProxyGraphics = true;
            }
        }
    }

    if (!hasCopyingSource) {
        makeSelectedItemAsGroup(selecteds);
    }
    if (dataPtr->scrollTimer) {
        dataPtr->scrollTimer->stop();
        delete dataPtr->scrollTimer;
        dataPtr->scrollTimer = nullptr;
    }

    // 视图把鼠标中键按下的事件转成了鼠标左键事件(用于按住鼠标中键时可以拖动画板)
    // 所以只能通过视图类接口去获取鼠标中键是否按下了
    bool isMiddlePressed = dataPtr->canvasView->isMiddleButtonPressed();

    auto keyModify = mouseEvent->modifiers();
    //  画板处于可修改和正常操作模式下时允许创建子系统
    // 有键盘按下时不允许创建子系统
    QPointF subSystemClickPos = dataPtr->canvasContext->subSystemClickPos();
    if (dataPtr->canvasContext && dataPtr->canvasContext->canModify()
        && dataPtr->canvasContext->getInteractionMode() == kNormal && !subSystemClickPos.isNull()
        && dataPtr->canvasContext->type() != CanvasContext::kUserDefinedFrameType && keyModify == Qt::NoModifier
        && !isMiddlePressed && dataPtr->canvasContext->type() != CanvasContext::kElecUserDefinedType
        && dataPtr->canvasContext->type() != CanvasContext::kElectricalType) {
        QPointF scenePos = mouseEvent->scenePos();
        double leftXPos = subSystemClickPos.x() < scenePos.x() ? subSystemClickPos.x() : scenePos.x();
        double topYPos = subSystemClickPos.y() < scenePos.y() ? subSystemClickPos.y() : scenePos.y();
        double rightXPos = subSystemClickPos.x() > scenePos.x() ? subSystemClickPos.x() : scenePos.x();
        double bottomYPos = subSystemClickPos.y() > scenePos.y() ? subSystemClickPos.y() : scenePos.y();
        QRectF areaRect = QRectF(QPointF(leftXPos, topYPos), QPointF(rightXPos, bottomYPos));
        if (qAbs(areaRect.width()) > 60 && qAbs(areaRect.height()) > 60) {
            if (selecteds.size() <= 0) {
                // 创建空子系统
                if (!dataPtr->subsystemAreaGraphics) {
                    dataPtr->subsystemAreaGraphics = new SubsystemAreaGraphics(nullptr);
                    dataPtr->subsystemAreaGraphics->setBoundingRect(areaRect);
                    addItem(dataPtr->subsystemAreaGraphics);
                }
                if (!dataPtr->subsystemAreaGraphics->isVisible()) {
                    dataPtr->subsystemAreaGraphics->setBoundingRect(areaRect);
                    dataPtr->subsystemAreaGraphics->setVisible(true);
                }
                dataPtr->subsystemAreaGraphics->setZValue(0xffff);
                dataPtr->canvasContext->setSubSystemClickPos(QPointF());

            } else {
                if (hasProxyGraphics) {
                    // 根据框选的内容创建子系统/构造系统
                    if (!dataPtr->subsystemAreaGraphics) {
                        dataPtr->subsystemAreaGraphics = new SubsystemAreaGraphics(nullptr);
                        dataPtr->subsystemAreaGraphics->setBoundingRect(areaRect);
                        addItem(dataPtr->subsystemAreaGraphics);
                    }
                    if (!dataPtr->subsystemAreaGraphics->isVisible()) {
                        dataPtr->subsystemAreaGraphics->setBoundingRect(areaRect);
                        dataPtr->subsystemAreaGraphics->setVisible(true);
                    }
                    dataPtr->subsystemAreaGraphics->setZValue(0xffff);
                    dataPtr->canvasContext->setSubSystemClickPos(QPointF());
                }
            }
            dataPtr->sourceNamePositionCommand = nullptr;
            dataPtr->positionCommand = nullptr;
        }
    }

    ICanvasScene::mouseReleaseEvent(mouseEvent);
}

void CanvasSceneDefaultImpl::keyPressEvent(QKeyEvent *event)
{
    if (dataPtr->subsystemAreaGraphics && dataPtr->subsystemAreaGraphics->isVisible()) {
        dataPtr->subsystemAreaGraphics->setVisible(false);
    }
    QPointF offset(0, 0);
    int step = 10;
    switch (event->key()) {
    case Qt::Key_Up:
        offset.setY(-step);
        break;
    case Qt::Key_Down:
        offset.setY(step);
        break;
    case Qt::Key_Left:
        offset.setX(-step);
        break;
    case Qt::Key_Right:
        offset.setX(step);
        break;
    default:
        ICanvasScene::keyPressEvent(event);
        return;
    }
    if (dataPtr->itemGroup /* && dataPtr->itemGroup->childItems().size() == selectedItems().size() */) {
        dataPtr->itemGroup->moveBy(offset.x(), offset.y());
        dataPtr->itemGroup->dealUndoRedo(offset);
        return;
    }
    QList<ConnectorWireGraphicsObject *> wires;
    QList<QGraphicsItem *> selectedItem = selectedItems();
    if (selectedItem.size() > 0 && getCanvasContext()->getInteractionMode() != kAnnotation) {
        QList<PortGraphicsObject *> outputPortList;
        PositionCommand *command = new PositionCommand(this);
        QListIterator<QGraphicsItem *> iter(selectedItem);

        QMap<QString, PConnectorWireContext> lastWiresContext;
        while (iter.hasNext()) {
            QGraphicsItem *item = iter.next();
            GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
            if (layer) {
                if (layer->type() == kConnectorWireGraphics) {
                    ConnectorWireGraphicsObject *wire = dynamic_cast<ConnectorWireGraphicsObject *>(layer);
                    if (wire && wire->isSourceBothSelected()) {
                        wire->setFlag(QGraphicsItem::ItemIsMovable, true);
                        QString wireUUID = wire->id();
                        lastWiresContext[wireUUID] =
                                PConnectorWireContext(new ConnectorWireContext(*(wire->getConnectorWireContext())));
                        wires.append(wire);
                    }
                }
            }
        }
        iter.toFront();
        while (iter.hasNext()) {
            QGraphicsItem *item = iter.next();
            GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
            if (layer) {
                // 连接线不响应键盘移动
                if (layer->type() == kConnectorWireGraphics) {
                    ConnectorWireGraphicsObject *wire = dynamic_cast<ConnectorWireGraphicsObject *>(layer);
                    if (wire && wire->isSourceBothSelected()) {
                        wire->moveBy(offset.x(), offset.y());
                        QPointF wirePos = wire->pos();
                        QPolygonF points = wire->getPoints();
                        int pointsSize = points.size();
                        for (int i = 0; i < pointsSize; i++) {
                            points[i] = points[i] + wirePos;
                        }
                        wire->setPoints(points);
                        wire->cachePoints();
                        wire->setPos(QPointF(0, 0));
                    }
                } else {
                    layer->moveBy(offset.x(), offset.y());
                    command->addPositionOffset(layer->id(), offset);
                    TransformProxyGraphicsObject *proxyGraphics = layer->getTransformProxyGraphicsObject();
                    if (proxyGraphics) {
                        auto sourceProxy = proxyGraphics->getSourceProxy();
                        if (sourceProxy) {
                            sourceProxy->setPos(layer->pos());
                        }
                        // 获取移动图元的端口列表
                        auto portLists = proxyGraphics->getPortGraphicsObjectList();
                        for each (auto port in portLists) {
                            if (!port) {
                                continue;
                            }
                            auto portContext = port->getPortContext();
                            if (portContext && portContext->type() == PortOutputType) {
                                outputPortList.append(port);
                            }
                        }
                    }
                }
            }
        }
        QMap<QString, PConnectorWireContext> nowWiresContext;
        for each (auto wire in wires) {
            if (wire) {
                wire->setFlag(QGraphicsItem::ItemIsMovable, false);
                QString wireID = wire->id();
                nowWiresContext[wireID] =
                        PConnectorWireContext(new ConnectorWireContext(*(wire->getConnectorWireContext())));
            }
        }
        command->setConnectorWireContext(lastWiresContext, nowWiresContext);
        // 更新移动图元关联的输出端口的分支点
        for each (auto port in outputPortList) {
            if (port) {
                // 输出端口移动了，重新计算输出端口所有连接线的分支点
                port->calcuteConnectorWireBranchPoints();
            }
        }
        if (!command->isEmpty()) {
            dataPtr->undoStack->push(command);
        } else {
            delete command;
        }
    } else {
        ICanvasScene::keyPressEvent(event);
    }
}

void CanvasSceneDefaultImpl::keyReleaseEvent(QKeyEvent *event)
{
    resetSubsystemGraphics();
    ICanvasScene::keyReleaseEvent(event);
}

void CanvasSceneDefaultImpl::onSourceNamePositionChanged(QPointF pointDelta)
{
    bool isIgnoreCommand = getUndoStack()->getIsIgnoreCommand();
    if (dataPtr->isMouseLeftPressed && !dataPtr->isTextNameChanged && !isIgnoreCommand) {
        if (nullptr == dataPtr->sourceNamePositionCommand) {
            dataPtr->sourceNamePositionCommand = new SourceNamePositionCommand(this);
        }
        TransformProxyGraphicsObject *transformProxy = dynamic_cast<TransformProxyGraphicsObject *>(sender());
        if (transformProxy) {
            dataPtr->sourceNamePositionCommand->addPositionOffset(transformProxy->id(), pointDelta);
        }
    }
    dataPtr->isTextNameChanged = false;
}

void CanvasSceneDefaultImpl::onScrollTimerOut()
{
    QList<QGraphicsView *> views = this->views();
    if (views.size() > 0) {
        QPointF viewPos = views[0]->mapFromGlobal(QCursor::pos());
        QPointF scenePos = views[0]->mapToScene(viewPos.toPoint());
        autoScroll(scenePos);
    }
}

void CanvasSceneDefaultImpl::onTransformProxyTransformChanged(QRectF rect, int angle)
{
    refreshTransformProxyGraphicsPainterPath();
}

void CanvasSceneDefaultImpl::onNewConnectWireShouldBeCreate(PortGraphicsObject *port1, PortGraphicsObject *port2)
{
    if (!port1 || !port2 || !dataPtr->canvasContext) {
        return;
    }
    ConnectorWireGraphicsObject *wire = new ConnectorWireGraphicsObject(this);
    wire->linkPortGraphicsObject(port1);
    wire->linkPortGraphicsObject(port2);
    if (wire->isConnectedWithPort()) {
        // 添加到AddConnectorWireCommand
        addConnectorWireGraphicsObject(wire, true);
        // 连接成功，把连接线上下文添加到画板
        dataPtr->canvasContext->addConnectWireContext(wire->getConnectorWireContext());
    }
}

void CanvasSceneDefaultImpl::onTransformProxySelectedChanged(bool selected)
{
    TransformProxyGraphicsObject *proxy = qobject_cast<TransformProxyGraphicsObject *>(sender());
    if (proxy) {
        if (selected) {
            dataPtr->selectedTransformProxys.push_back(proxy);
        } else {
            dataPtr->selectedTransformProxys.removeAll(proxy);
        }
        updateActionStatus();
    }
}

void CanvasSceneDefaultImpl::onDeleteSource(QString uuid)
{
    if (dataPtr->canvasContext) {
        deleteSourceFromScene(dataPtr->canvasContext->getSource(uuid));
    }
}

void CanvasSceneDefaultImpl::onCanvasPropertyChanged(const QString &name, const QVariant &oldValue,
                                                     const QVariant &newValue)
{
    if (!dataPtr->graphicsLayerManager) {
        return;
    }
    if (name == HIGHLIGHT_PROPERTY_SOURCELIST_KEY) {
        // 高亮资源列表变更，将之前高亮的资源状态重置
        if (oldValue.canConvert<QStringList>()) {
            QStringList oldSourceIDS = oldValue.toStringList();
            for (int i = 0; i < oldSourceIDS.size(); i++) {
                QString id = oldSourceIDS[i];
                if (id.isEmpty()) {
                    continue;
                }
                TransformProxyGraphicsObject *proxy = getTransformProxyGraphicsByID(id);
                if (proxy) {
                    proxy->getLayerProperty().setHightLight(false);
                }
            }
        }
        if (newValue.canConvert<QStringList>()) {
            QStringList newSourceIDS = newValue.toStringList();
            for (int i = 0; i < newSourceIDS.size(); i++) {
                QString id = newSourceIDS[i];
                if (id.isEmpty()) {
                    continue;
                }
                TransformProxyGraphicsObject *proxy = getTransformProxyGraphicsByID(id);
                if (proxy) {
                    proxy->getLayerProperty().setHightLight(true);
                }
            }
        }
    } else if (name == BASE_LAYER_VISIBLE_PROPERTY_KEY) {
        auto layers = dataPtr->graphicsLayerManager->getLayers(GraphicsLayer::kSourceGraphicsLayer);
        for each (auto layer in layers) {
            if (layer) {
                layer->getLayerProperty().setVisible(newValue.toBool());
            }
        }
        layers = dataPtr->graphicsLayerManager->getLayers(GraphicsLayer::kPortGraphicsLayer);
        for each (auto layer in layers) {
            if (layer) {
                layer->getLayerProperty().setVisible(newValue.toBool());
            }
        }
        layers = dataPtr->graphicsLayerManager->getLayers(GraphicsLayer::kBackgroundLayer);
        for each (auto layer in layers) {
            if (layer) {
                layer->getLayerProperty().setVisible(newValue.toBool());
            }
        }
        layers = dataPtr->graphicsLayerManager->getLayers(GraphicsLayer::kForegroundLayer);
        for each (auto layer in layers) {
            if (layer) {
                layer->getLayerProperty().setVisible(newValue.toBool());
            }
        }

    } else if (name == SOURCENAME_VISIBLE_PROPERTY_KEY) {
        // 资源名称图层可见性变化
        auto layers = dataPtr->graphicsLayerManager->getLayers(GraphicsLayer::kSourceNameGraphicsLayer);
        for each (auto layer in layers) {
            if (layer) {
                layer->getLayerProperty().setVisible(newValue.toBool());
            }
        }
    } else if (name == ANNOTATION_VISIBLE_PROPERTY_KEY) {
        auto layers = dataPtr->graphicsLayerManager->getLayers(GraphicsLayer::kCommentGraphicsLayer);
        for each (auto layer in layers) {
            if (layer) {
                layer->getLayerProperty().setVisible(newValue.toBool());
            }
        }
    } else if (name == LEGEND_LAYER_VISIBLE_PROPERTY_KEY) {
        if (dataPtr->copyRightGraphicsObject) {
            dataPtr->copyRightGraphicsObject->getLayerProperty().setVisible(newValue.toBool());
        }
    } else if (name == BUSBAR_PHAPSE_VISIBLE_PROPERTY_KEY) {
        auto layers = dataPtr->graphicsLayerManager->getLayers(GraphicsLayer::kBusbarPhaseGraphicsLayer);
        for each (auto layer in layers) {
            if (layer) {
                layer->getLayerProperty().setVisible(newValue.toBool());
            }
        }
    } else if (name == FONT_SOURCE_NAME_PROPERTY_KEY) {
        auto layers = dataPtr->graphicsLayerManager->getLayers(GraphicsLayer::kSourceNameGraphicsLayer);
        for each (auto layer in layers) {
            if (layer) {
                layer->getLayerProperty().setFont(newValue.value<QFont>());
            }
        }
    } else if (name == CONNECTOR_WIRE_VISIBLE_PROPERTY_KEY) {
        auto layers = dataPtr->graphicsLayerManager->getLayers(GraphicsLayer::kConnectorWireGraphicsLayer);
        for each (auto layer in layers) {
            if (layer) {
                layer->getLayerProperty().setVisible(newValue.toBool());
            }
        }
    } else if (name == HIGHLIGHT_GOTOFROM_PROPERTY_SOURCELIST_KEY) {
        if (oldValue.canConvert<QStringList>()) {
            QStringList oldSourceIDS = oldValue.toStringList();
            for (int i = 0; i < oldSourceIDS.size(); i++) {
                QString id = oldSourceIDS[i];
                if (id.isEmpty()) {
                    continue;
                }
                TransformProxyGraphicsObject *proxy = getTransformProxyGraphicsByID(id);
                if (proxy) {
                    proxy->getLayerProperty().setGotoFromHightLight(false);
                }
            }
        }
        if (newValue.canConvert<QStringList>()) {
            QStringList newSourceIDS = newValue.toStringList();
            for (int i = 0; i < newSourceIDS.size(); i++) {
                QString id = newSourceIDS[i];
                if (id.isEmpty()) {
                    continue;
                }
                TransformProxyGraphicsObject *proxy = getTransformProxyGraphicsByID(id);
                if (proxy) {
                    proxy->getLayerProperty().setGotoFromHightLight(true);
                }
            }
        }
    }

    qDebug() << "画板属性变更之后的值:" << dataPtr->canvasContext->getCanvasProperty().getProperties();
}

void CanvasSceneDefaultImpl::onCanvasGridFlagChanged(int oldFlag, int newFlag)
{
    invalidate(sceneRect(), QGraphicsScene::BackgroundLayer);
}

void CanvasSceneDefaultImpl::onSceneSelectionsChanged()
{
    auto selecteds = selectedItems();
    if (selecteds.isEmpty()) {
        ActionManager::getInstance().getAction(ActionManager::Delete)->setEnabled(false);
        ActionManager::getInstance().getCopyAbleActionGroup()->setEnabled(false);
        ActionManager::getInstance().getTransformActionGroup()->setEnabled(false);
    }
    if (selecteds.size() == 1) {
        TransformProxyGraphicsObject *trans = dynamic_cast<TransformProxyGraphicsObject *>(selecteds[0]);
        if (trans) {
            setHightLightTrans(trans->getSourceProxy());
        }
    } else {
        if (dataPtr->canvasContext) {
            CanvasProperty &canvasProperty = dataPtr->canvasContext->getCanvasProperty();
            canvasProperty.setGotoFromHighLightSourcceList(QStringList());
        }
    }
}

void CanvasSceneDefaultImpl::onSceneRectChanged(const QRectF &rect)
{
    getCanvasContext()->setSenceRectf(rect);
    if (dataPtr->copyRightGraphicsObject) {
        QRectF copyRightRc = dataPtr->copyRightGraphicsObject->boundingRect();
        if (!rect.contains(dataPtr->copyRightGraphicsObject->mapRectToScene(copyRightRc))) {
            QPointF pos = QPointF(rect.right() - copyRightRc.width() - 10,
                                  rect.bottom() - copyRightRc.height() - 10); // 减10为了图例与scene边界有间隔
            pos = Utility::pointAlignmentToGrid(pos, getGridSpace());
            dataPtr->copyRightGraphicsObject->setPos(pos);
        }
    }

    if (!dataPtr->canvasContext) {
        return;
    }
    dataPtr->backgroundPoints.clear();
    dataPtr->backgroundLines.clear();
    qreal xleft, xright, ytop, ybottom;
    xleft = rect.left();
    xright = rect.left() + rect.width();
    ytop = rect.top();
    ybottom = rect.top() + rect.height();
    QRectF rectScene = sceneRect();
    int gridSpace = dataPtr->canvasContext->gridSpace();

    for (int x = rectScene.left(); x < rectScene.right(); x += gridSpace) {
        if (x >= xleft && x <= xright) {
            dataPtr->backgroundLines.push_back(QLine(x, rectScene.top(), x, rectScene.bottom()));
        }
    }
    for (int y = rectScene.top(); y < rectScene.bottom(); y += gridSpace) {
        if (y >= ytop && y <= ybottom) {
            dataPtr->backgroundLines.push_back(QLine(rectScene.left(), y, rectScene.right(), y));
        }
    }

    for (int x = rectScene.left(); x < rectScene.right(); x += gridSpace) {
        if (x >= xleft && x <= xright) {
            for (int y = rectScene.top(); y < rectScene.bottom(); y += gridSpace) {
                if (y >= ytop && y <= ybottom) {
                    dataPtr->backgroundPoints.push_back(QPoint(x, y));
                }
            }
        }
    }
}

void CanvasSceneDefaultImpl::onBreakLinkLine()
{
    if (selectedItems().size() > 0) {
        DeleteCommand *command = new DeleteCommand(this);
        QListIterator<QGraphicsItem *> iter(selectedItems());
        while (iter.hasNext()) {
            QGraphicsItem *item = iter.next();
            TransformProxyGraphicsObject *layer = dynamic_cast<TransformProxyGraphicsObject *>(item);
            if (!layer)
                continue;
            QList<PortGraphicsObject *> portList = layer->getPortGraphicsObjectList();
            foreach (PortGraphicsObject *port, portList) {
                QList<ConnectorWireGraphicsObject *> connectList = port->getLinkedConnectorWireList();
                foreach (ConnectorWireGraphicsObject *connectLine, connectList) {
                    if (connectLine) {
                        if (dataPtr->itemGroup) {
                            dataPtr->itemGroup->removeFromGroup(connectLine);
                        }
                        command->addDeleteGraphics(connectLine);
                    }
                }
            }
        }
        if (dataPtr->undoStack) {
            dataPtr->undoStack->push(command);
        }
    }
}

void CanvasSceneDefaultImpl::onUpdateModule()
{
    if (selectedItems().size() <= 0 || !dataPtr->canvasContext)
        return;
    QString prototypeName, name, oldUuid;
    QPointF centerPoint;
    QList<QSharedPointer<ConnectorWireContext>> connectWireContextList;
    QList<QSharedPointer<PortContext>> portList;
    // 删除模块
    DeleteCommand *command = new DeleteCommand(this);
    QListIterator<QGraphicsItem *> selectIter(selectedItems());
    while (selectIter.hasNext()) {
        QGraphicsItem *item = selectIter.next();
        TransformProxyGraphicsObject *trans = dynamic_cast<TransformProxyGraphicsObject *>(item);
        if (trans) {
            QSharedPointer<SourceProxy> source = trans->getSourceProxy();
            prototypeName = source->prototypeName();
            name = source->name();
            oldUuid = source->uuid();
            centerPoint = trans->sceneBoundingRect().center();
            auto portListGraphics = trans->getPortGraphicsObjectList();
            for each (PortGraphicsObject *port in portListGraphics) {
                auto wireGraphics = port->getLinkedConnectorWireList();
                for each (ConnectorWireGraphicsObject *wire in wireGraphics) {
                    connectWireContextList.append(wire->getConnectorWireContext());
                }
            }
            portList = source->portList();
        }
        GraphicsLayer *layer = dynamic_cast<GraphicsLayer *>(item);
        if (layer) {
            command->addDeleteGraphics(layer);
        }
    }
    if (dataPtr->undoStack) {
        dataPtr->undoStack->push(command);
    }
    // 创建新的模块
    BusinessHooksServer *hookserver = dataPtr->canvasContext->getBusinessHooksServer();
    if (!hookserver)
        return;
    QString uuid = hookserver->createNewBlock(prototypeName, centerPoint, true, name);
    QSharedPointer<SourceProxy> newSource = dataPtr->canvasContext->getSource(uuid);
    TransformProxyGraphicsObject *newTrans = getTransformProxyGraphicsByID(uuid);

    // 创建新的连接线
    if (!newSource || !newTrans)
        return;
    QList<QSharedPointer<PortContext>> newPortList = newSource->portList();
    QMap<QString, PPortContext> oldNameMap;
    QMap<QString, PPortContext> newNameMap;
    for each (auto port in portList) {
        oldNameMap[port->name()] = port;
    }
    for each (auto port in newPortList) {
        newNameMap[port->name()] = port;
    }
    QSet<QString> removePortSet;
    // 先按名称匹配一次
    for (int i = 0; i < connectWireContextList.size();) {
        PConnectorWireContext wireContext = connectWireContextList[i];
        QString srcPortUuid = wireContext->srcPortID();
        QString dstPortUuid = wireContext->dstPortID();
        PortGraphicsObject *srcPort = nullptr;
        PortGraphicsObject *dstPort = nullptr;
        auto dstSource = wireContext->dstSource();
        auto srcSource = wireContext->srcSource();
        bool isCreateNewWire = false;
        if (!dstSource || !srcSource) {
            ++i;
            continue;
        }
        if (dstSource->uuid() == oldUuid) {
            for each (auto port in portList) {
                if (port->uuid() == dstPortUuid && port->type() == "input" && newNameMap.contains(port->name())) {
                    auto srcTrans = getTransformProxyGraphicsByID(srcSource->uuid());
                    if (srcTrans) {
                        srcPort = srcTrans->getPortGraphicsObject(srcPortUuid);
                    }
                    dstPort = newTrans->getPortGraphicsObject(newNameMap[port->name()]->uuid());
                    onNewConnectWireShouldBeCreate(srcPort, dstPort);
                    isCreateNewWire = true;
                    removePortSet.insert(newNameMap[port->name()]->uuid());
                    removePortSet.insert(dstPortUuid);
                    break;
                }
            }
        } else if (srcSource->uuid() == oldUuid) {
            for each (auto port in portList) {
                if (port->uuid() == srcPortUuid && port->type() == "output" && newNameMap.contains(port->name())) {
                    srcPort = newTrans->getPortGraphicsObject(newNameMap[port->name()]->uuid());
                    auto dstTrans = getTransformProxyGraphicsByID(dstSource->uuid());
                    if (dstTrans) {
                        dstPort = dstTrans->getPortGraphicsObject(dstPortUuid);
                    }
                    onNewConnectWireShouldBeCreate(srcPort, dstPort);
                    isCreateNewWire = true;
                    removePortSet.insert(newNameMap[port->name()]->uuid());
                    removePortSet.insert(srcPortUuid);
                    break;
                }
            }
        }
        if (isCreateNewWire) {
            connectWireContextList.removeAt(i);
        } else {
            ++i;
        }
    }
    QMap<QString, int> oldInputMap, oldOutputMap;
    QMap<int, QString> newInputMap, newOutputMap;
    for each (auto port in portList) {
        if (removePortSet.contains(port->uuid()))
            continue;
        if (port->type() == "input") {
            oldInputMap[port->uuid()] = oldInputMap.size() + 1;
        } else {
            oldOutputMap[port->uuid()] = oldOutputMap.size() + 1;
        }
    }
    for each (auto port in newPortList) {
        if (removePortSet.contains(port->uuid()))
            continue;
        if (port->type() == "input") {
            newInputMap[newInputMap.size() + 1] = port->uuid();
        } else {
            newOutputMap[newOutputMap.size() + 1] = port->uuid();
        }
    }
    for each (auto wireContext in connectWireContextList) {
        QString srcPortUuid = wireContext->srcPortID();
        QString dstPortUuid = wireContext->dstPortID();
        PortGraphicsObject *srcPort = nullptr;
        PortGraphicsObject *dstPort = nullptr;
        auto dstSource = wireContext->dstSource();
        auto srcSource = wireContext->srcSource();
        if (!dstSource || !srcSource) {
            continue;
        }
        if (dstSource->uuid() == oldUuid && oldInputMap[dstPortUuid] <= newInputMap.size()) {
            auto *srcTrans = getTransformProxyGraphicsByID(srcSource->uuid());
            if (srcTrans) {
                srcPort = srcTrans->getPortGraphicsObject(srcPortUuid);
            }
            dstPort = newTrans->getPortGraphicsObject(newInputMap[oldInputMap[dstPortUuid]]);
        } else if (srcSource->uuid() == oldUuid && oldOutputMap[srcPortUuid] <= newOutputMap.size()) {
            srcPort = newTrans->getPortGraphicsObject(newOutputMap[oldOutputMap[srcPortUuid]]);
            auto *dstTrans = getTransformProxyGraphicsByID(dstSource->uuid());
            if (dstTrans) {
                dstPort = dstTrans->getPortGraphicsObject(dstPortUuid);
            }
        }
        onNewConnectWireShouldBeCreate(srcPort, dstPort);
    }
}

QList<TransformProxyGraphicsObject *> CanvasSceneDefaultImpl::transProxyAtRectF(QRectF rect)
{
    QList<TransformProxyGraphicsObject *> containList;
    for each (auto trans in getTransformProxyGraphicsList()) {
        if (trans && trans->getTransformSceneRect().intersects(rect)) {
            containList.push_back(trans);
        }
    }
    return containList;
}

AutoConnectionPromptGraphics *CanvasSceneDefaultImpl::getAutoConnectionPromptGraphics()
{
    return dataPtr->autoConnectionPromptGraphics;
}

AutoConnectionPromptGraphics *CanvasSceneDefaultImpl::createAutoConnectionPromptGraphics()
{
    if (dataPtr->autoConnectionPromptGraphics) {
        return dataPtr->autoConnectionPromptGraphics;
    }
    dataPtr->autoConnectionPromptGraphics = new AutoConnectionPromptGraphics(this, nullptr);
    addItem(dataPtr->autoConnectionPromptGraphics);
    dataPtr->autoConnectionPromptGraphics->setZValue(1000);
    return dataPtr->autoConnectionPromptGraphics;
}

void CanvasSceneDefaultImpl::setMouseClickedPort(PortGraphicsObject *port)
{
    dataPtr->mouseClickedPort = port;
}

PortGraphicsObject *CanvasSceneDefaultImpl::getMouseClickedPort()
{
    return dataPtr->mouseClickedPort;
}

bool CanvasSceneDefaultImpl::getTransformProxyChanging()
{
    return dataPtr->hasItemMoving;
}

void CanvasSceneDefaultImpl::setTransformProxyChanging(bool isMoving)
{
    if (dataPtr->hasItemMoving != isMoving) {
        dataPtr->hasItemMoving = isMoving;
        if (!isMoving) {
            refreshTransformProxyGraphicsPainterPath();
            emit transformProxyChangedSignal();
        }
    }
}

void CanvasSceneDefaultImpl::nameChanged(QString id, QPair<QString, QString> name)
{
    if (nullptr == dataPtr->sourceNamePositionCommand) {
        dataPtr->sourceNamePositionCommand = new SourceNamePositionCommand(this);
    }
    dataPtr->sourceNamePositionCommand->addnameChanged(id, name);
    dataPtr->undoStack->push(dataPtr->sourceNamePositionCommand);
    dataPtr->sourceNamePositionCommand = nullptr;
    dataPtr->isTextNameChanged = true;
}

QVariantMap CanvasSceneDefaultImpl::wireComputation(const QSharedPointer<WireParameters> wireParameter)
{
    if (!wireParameter) {
        return QVariantMap();
    }

    QPolygonF points = wireParameter->pathResult;
    if (wireParameter->algorithmName == ManhattaAlgorithmName) {
        ManhattaConnectorWireAlgorithm algorithm;

        if (wireParameter->autoCalculate) {
            points = algorithm.getAlgorithmPath(wireParameter->startPos, wireParameter->startExternalPos,
                                                wireParameter->endPos, wireParameter->endExternalPos,
                                                wireParameter->obstacleAreaPath);
        } else {
            int type = 1;
            QPointF movingPos = wireParameter->startPos;
            if (wireParameter->isEndMoving) {
                type = 2;
                movingPos = wireParameter->endPos;
            }
            bool adjusted = algorithm.portMoving(movingPos, type, wireParameter->pathResult);
            if (!adjusted) {
                wireParameter->autoCalculate = true;
                points = algorithm.getAlgorithmPath(wireParameter->startPos, wireParameter->startExternalPos,
                                                    wireParameter->endPos, wireParameter->endExternalPos,
                                                    wireParameter->obstacleAreaPath);
            } else {
                points = wireParameter->pathResult;
            }
        }
    } else if (wireParameter->algorithmName == BezierAlogrithmName) {
        BezierConnectorWireAlgorithm algorithm;
        points = algorithm.getAlgorithmPath(wireParameter->startPos, wireParameter->startExternalPos,
                                            wireParameter->endPos, wireParameter->endExternalPos);
    } else if (wireParameter->algorithmName == StraightLineAlgorithmName) {
        StraightLineConnectorWireAlgorithm algorithm;
        points = algorithm.getAlgorithmPath(wireParameter->startPos, wireParameter->startExternalPos,
                                            wireParameter->endPos, wireParameter->endExternalPos);
    }

    QVariantMap result;
    result["id"] = wireParameter->wireUUID;
    result["points"] = points.toPolygon();
    result["autoCalculate"] = wireParameter->autoCalculate;
    return result;
}

void CanvasSceneDefaultImpl::refreshConnectorWires(QList<QSharedPointer<WireParameters>> wireParameters)
{
    if (wireParameters.isEmpty()) {
        return;
    }
#ifdef _DEBUG
    DEBUG_COST_TIME();
#endif
    // 更新连接线之前，刷新所有TransformProxy区域路径
    refreshTransformProxyGraphicsPainterPath();
    QPainterPath obstaclePath = getTransformProxyGraphicsPainterPath();
    for each (auto parameter in wireParameters) {
        if (parameter) {
            parameter->obstacleAreaPath = obstaclePath;
            if (dataPtr->canvasContext && !dataPtr->canvasContext->isSubsystemInitialized()) {
                // 子系统初始化的时候，会调整输入输出位置，此时输入输出可能会有旋转移动变换，不处理碰撞检测，否则会出现本来可以直线连接的，会出现直角拐点
                parameter->obstacleAreaPath = QPainterPath();
            }
        }
    }

    QList<QVariantMap> results;
    // 如果只有一根连接线需要更新，那么就在主线程中执行，避免上下文切换
    if (wireParameters.size() == 1) {
        auto ret = wireComputation(wireParameters[0]);
        results.push_back(ret);
    } else {
        QFuture<QVariantMap> futures = QtConcurrent::mapped(wireParameters, wireComputation);
        futures.waitForFinished();
        results = futures.results();
    }
    QList<PortGraphicsObject *> outputPortList;
    for each (auto result in results) {
        if (result.contains("id") && result.contains("points") && result.contains("autoCalculate")) {
            QString wireID = result["id"].value<QString>();
            QPolygon points = result["points"].value<QPolygon>();
            bool autoCaculate = result["autoCalculate"].value<bool>();
            auto wire = getConnectorWireGraphicsByID(wireID);
            if (wire) {
                wire->setPoints(points);
                wire->setAutoCalculation(autoCaculate);
                outputPortList.push_back(wire->getOutputTypePortGraphics());
            }
        }
    }
    for each (auto port in outputPortList) {
        if (port) {
            port->cleanConnectorWireBranchPoints();
            port->calcuteConnectorWireBranchPoints();
        }
    }
}

void CanvasSceneDefaultImpl::setPromptWirePorts(PortGraphicsObject *port1, PortGraphicsObject *port2)
{
    if (dataPtr->promptWire) {
        dataPtr->promptWire->setPorts(port1, port2);
    }
}

bool CanvasSceneDefaultImpl::isIgnoreLayerManager()
{
    return dataPtr->isIgnoreLayerManager;
}

void CanvasSceneDefaultImpl::setIgnoreLayerManager(bool ignore)
{
    dataPtr->isIgnoreLayerManager = ignore;
}

void CanvasSceneDefaultImpl::createNewConnectWireByPrompt()
{
    if (dataPtr->promptWire) {
        auto portList = dataPtr->promptWire->getPortList();
        onNewConnectWireShouldBeCreate(portList[0].data(), portList[1].data());
    }
}

void CanvasSceneDefaultImpl::freeSource()
{
    dataPtr->connectorGraphicsMap.clear();
    dataPtr->proxyGraphicsMap.clear();
    clear();
}

void CanvasSceneDefaultImpl::refreshInputAndOutPutIndex()
{
    if (!dataPtr->canvasContext) {
        return;
    }
    auto inputMap = dataPtr->canvasContext->getInputSourceMap();
    auto outputMap = dataPtr->canvasContext->getOutputSourceMap();
    int inputIndex = 1;
    int ouputIndex = 1;
    for each (auto iter in inputMap) {
        SourceProperty &sourceProperty = iter->getSourceProperty();
        sourceProperty.setInputOutputDisplayIndex(inputIndex);
        ++inputIndex;
    }
    for each (auto iter in outputMap) {
        SourceProperty &sourceProperty = iter->getSourceProperty();
        sourceProperty.setInputOutputDisplayIndex(ouputIndex);
        ++ouputIndex;
    }
}

void CanvasSceneDefaultImpl::resetSubsystemGraphics()
{
    if (dataPtr->subsystemAreaGraphics) {
        dataPtr->subsystemAreaGraphics->setVisible(false);
        dataPtr->canvasContext->setSubSystemClickPos(QPointF());
    }
}

void CanvasSceneDefaultImpl::setHightLightTrans(PSourceProxy source)
{
    if (!dataPtr->canvasContext) {
        return;
    }
    QStringList sourceIDS;
    if (!source || (source->prototypeName() != "Goto" && source->prototypeName() != "From")) {
        dataPtr->canvasContext->getCanvasProperty().setGotoFromHighLightSourcceList(sourceIDS);
        return;
    }
    SourceProperty &sourceProperty = source->getSourceProperty();
    QString sourcePrototypeName = source->prototypeName();
    QString sourceDisplayData = sourceProperty.getDisplayData().toString();
    for each (auto src in dataPtr->canvasContext->getAllSource()) {
        QString tempPrototypeName = src->prototypeName();
        QString tempDisplayData = src->getSourceProperty().getDisplayData().toString();
        if ((tempPrototypeName == "Goto" || tempPrototypeName == "From") && tempPrototypeName != sourcePrototypeName
            && tempDisplayData == sourceDisplayData) {
            sourceIDS << src->uuid();
        }
    }
    dataPtr->canvasContext->getCanvasProperty().setGotoFromHighLightSourcceList(sourceIDS);
}

QRectF CanvasSceneDefaultImpl::getCopyRightRect()
{
    if (!dataPtr->copyRightGraphicsObject) {
        return QRectF();
    }
    return dataPtr->copyRightGraphicsObject->mapToScene(dataPtr->copyRightGraphicsObject->boundingRect())
            .boundingRect();
}

QRectF CanvasSceneDefaultImpl::getAllItemUnitedRect()
{
    // 扩展sceneRect以包含所有图元
    QRectF itemsBoundingRect;
    for (auto trans : getTransformProxyGraphicsList()) {
        if (trans) {
            itemsBoundingRect = itemsBoundingRect.united(trans->mapRectToScene(trans->unitedChildsBoundingRect()));
        }
    }
    // return itemsBoundingRect.contains(QRectF(0,0,2239,1583)) ? itemsBoundingRect : QRectF(0,0,2239,1583);
    return itemsBoundingRect;
}

void CanvasSceneDefaultImpl::onRefreshCanvas()
{
    update();
}