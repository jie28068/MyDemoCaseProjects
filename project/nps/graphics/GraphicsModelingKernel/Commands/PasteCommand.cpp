#include "PasteCommand.h"
#include "BusinessHooksServer.h"
#include "CanvasContext.h"
#include "CanvasSceneDefaultImpl.h"
#include "NPSPropertyManager.h"
#include "SourceProxy.h"
#include "TransformItemGroup.h"
#include "TransformProxyGraphicsObject.h"
#include "Utility.h"

PasteCommand::PasteCommand(ICanvasScene *scene, QUndoCommand *parent /* = 0 */) : CanvasUndoCommand(scene, parent)
{
    isCut = false;
    isUndoRun = false;
}

void PasteCommand::loadClipboardMimeData(ClipBoardMimeData *mimeData)
{
    if (nullptr == mimeData) {
        return;
    }
    if (!canvasScene) {
        return;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }
    BusinessHooksServer *hookserver = canvasContext->getBusinessHooksServer();
    if (hookserver == nullptr) {
        return;
    }
    originalSourcesMap.clear();
    originalConnectorsMap.clear();
    QMap<QString, QSharedPointer<SourceProxy>> tmpsource = mimeData->getSourcesMap();
    foreach (QString key, tmpsource.keys()) {
        if (hookserver->enableCreate(mimeData->getCanvasType(), tmpsource[key])) {
            originalSourcesMap.insert(key, tmpsource[key]);
        }
    }
    originalConnectorsMap = mimeData->getConnectorsMap();
}

void PasteCommand::setCutFlag(bool value)
{
    isCut = value;
}

void PasteCommand::redo()
{
    if (!canvasScene) {
        return;
    }
    auto canvasContext = canvasScene->getCanvasContext();
    if (!canvasContext) {
        return;
    }
    auto businessHook = canvasContext->getBusinessHooksServer();
    if (!businessHook) {
        return;
    }
    if (canvasScene->getTransformItemGroup()) {
        canvasScene->getTransformItemGroup()->clear();
    }
    canvasScene->clearSelection();
    QVector<QString> allSourceName;

    CanvasSceneDefaultImpl *canvasDefault = dynamic_cast<CanvasSceneDefaultImpl *>(canvasScene);
    QPointF mouseClickedPos = canvasDefault->getMouseClickedPos();

    QMapIterator<QString, PSourceProxy> sourceIter(originalSourcesMap);
    QRectF unitedRect;
    while (sourceIter.hasNext()) {
        sourceIter.next();
        PSourceProxy s = sourceIter.value();
        QRectF sourceRect = QRectF(s->pos(), s->size());
        // sourceRect = s->getCombineTransform().map(sourceRect).boundingRect();
        unitedRect = unitedRect.united(sourceRect);
    }

    // 新复制的图元和原始图元的坐标偏移量
    QPointF distanceOffset = mouseClickedPos - unitedRect.topLeft();
    distanceOffset = Utility::pointAlignmentToGrid(distanceOffset, 10);
    // 粘贴产生的新资源映射表
    if (newSourcesMap.isEmpty()) {
        QMapIterator<QString, PSourceProxy> originalSourceIter(originalSourcesMap);
        QMapIterator<QString, PSourceProxy> originalSourceIters(originalSourcesMap);
        while (originalSourceIters.hasNext()) {
            originalSourceIters.next();
            allSourceName.push_back(originalSourceIters.value().data()->name());
        }
        while (originalSourceIter.hasNext()) {
            originalSourceIter.next();
            QString id = originalSourceIter.key();
            PSourceProxy originalSource = originalSourceIter.value();
            // 把新创建的资源的名称给业务层接口处理,然后改变资源的uuid
            qDebug() << originalSource->getSourceProperty().toJson();
            PSourceProxy newSource = PSourceProxy(new SourceProxy(*originalSource.data()));
            PCanvasContext canvasCtx = canvasScene->getCanvasContext();
            // 注释模块只有图片注释有名称 其他都不需要名称
            if (originalSource->moduleType() != GKD::SOURCE_MODULETYPE_ANNOTATION
                || originalSource->prototypeName() == ImageAnnotationStr) {
                businessHook->setNewSourceProxy(originalSource, newSource, allSourceName); // 重命名
            }
            newSource->setUUID(Utility::createUUID()); // 更新UUID
            newSource->setOriginalUUID(originalSource->uuid()); // 设置源UUID，表明是从哪一个souce拷贝构造出来的
            newSource->setPos(originalSource->pos() + distanceOffset); // 设置坐标

            newSource->resetPortSourceUUid();

            if (newSource->isSupportAddOrDelPort()) {
                newPortContext[newSource->uuid()] = newSource->portList();
            }

            newSourcesMap[newSource->uuid()] = newSource;          // 把新创建的资源添加到map映射表
            oldToNewSourceMap[originalSource->uuid()] = newSource; // 保存新旧数据的映射关系
        }
    }

    if (newConnectorsMap.isEmpty()) {
        QMapIterator<QString, PConnectorWireContext> originalConnectorIter(originalConnectorsMap);
        while (originalConnectorIter.hasNext()) {
            originalConnectorIter.next();
            PConnectorWireContext originalConnectorCtx = originalConnectorIter.value();
            PSourceProxy srcSource = originalConnectorCtx->srcSource();
            PSourceProxy dstSource = originalConnectorCtx->dstSource();

            if (!srcSource.isNull() && !dstSource.isNull()) {
                QString srcUUID = srcSource->uuid();
                QString dstUUID = dstSource->uuid();
                if (oldToNewSourceMap.contains(srcUUID) && oldToNewSourceMap.contains(dstUUID)) {
                    // 说明连接线两端的资源也同时会被粘贴创建
                    // 从原始连接线拷贝连接线上下文
                    PConnectorWireContext newConnectorCtx =
                            PConnectorWireContext(new ConnectorWireContext(*originalConnectorCtx.data()));
                    newConnectorCtx->setUUID(Utility::createUUID());
                    newConnectorCtx->setOriginalUUID(originalConnectorCtx->uuid());
                    // 连接线坐标点偏移
                    QPolygonF points = newConnectorCtx->points();
                    for (int i = 0; i < points.size(); i++) {
                        points[i] = points[i] + distanceOffset;
                    }
                    newConnectorCtx->setPoints(points);
                    PSourceProxy newSrcSource = oldToNewSourceMap[srcUUID];
                    PSourceProxy newDstSource = oldToNewSourceMap[dstUUID];

                    newConnectorCtx->setSrcSource(newSrcSource);
                    newConnectorCtx->setDstSource(newDstSource);

                    newConnectorsMap[newConnectorCtx->uuid()] = newConnectorCtx;
                }
            }
        }
    }

    // 创建资源图元
    QList<PSourceProxy> inSourceList, outSourceList;
    QMapIterator<QString, PSourceProxy> newSourceIter(newSourcesMap);
    while (newSourceIter.hasNext()) {
        newSourceIter.next();
        PSourceProxy newSource = newSourceIter.value();
        if (newPortContext.contains(newSource->uuid())) {
            newSource->resetPortList(newPortContext[newSource->uuid()]);
        }
        if (isCut) {
            newSource->setSourceCreateType(SourceProxy::kCreateCut);
        } else {
            newSource->setSourceCreateType(SourceProxy::kCreateCopy);
        }

        if (isUndoRun) {
            newSource->setSourceCreateType(SourceProxy::kCreateRevert);
        }
        if (newSource->prototypeName() == "In") {
            inSourceList.append(newSource);
            continue;
        }
        if (newSource->prototypeName() == "Out") {
            outSourceList.append(newSource);
            continue;
        }

        canvasScene->loadFromSourceProxy(newSource);
        GraphicsLayer *layer = canvasScene->getTransformProxyGraphicsBySource(newSource);
        if (layer) {
            layer->setSelected(true);
        }
    }
    qSort(inSourceList.begin(), inSourceList.end(), &PasteCommand::sortInOutSource);

    qSort(outSourceList.begin(), outSourceList.end(), &PasteCommand::sortInOutSource);

    foreach (auto newSource, inSourceList) {
        SourceProperty &newSourceProperty = newSource->getSourceProperty();
        newSourceProperty.setInputOutputIndex(-1);
        newSourceProperty.setInputOutputDisplayIndex(0);
        canvasScene->loadFromSourceProxy(newSource);
        GraphicsLayer *layer = canvasScene->getTransformProxyGraphicsBySource(newSource);
        if (layer) {
            layer->setSelected(true);
        }
    }

    foreach (auto newSource, outSourceList) {
        SourceProperty &newSourceProperty = newSource->getSourceProperty();
        newSourceProperty.setInputOutputIndex(-1);
        newSourceProperty.setInputOutputDisplayIndex(0);
        canvasScene->loadFromSourceProxy(newSource);
        GraphicsLayer *layer = canvasScene->getTransformProxyGraphicsBySource(newSource);
        if (layer) {
            layer->setSelected(true);
        }
    }

    // 创建连接线图元
    QMapIterator<QString, PConnectorWireContext> newConnectorIter(newConnectorsMap);
    while (newConnectorIter.hasNext()) {
        newConnectorIter.next();
        QString uuid = newConnectorIter.key();
        PConnectorWireContext newConnectorCtx = newConnectorIter.value();

        if (canvasScene->getConnectorWireGraphicsByID(uuid) != nullptr) {
            continue;
        }
        ConnectorWireGraphicsObject *graphics = new ConnectorWireGraphicsObject(canvasScene);
        graphics->loadFromContext(newConnectorCtx);

        if (graphics->isConnectedWithPort()) {
            // 连接成功，把连接线上下文添加到画板
            canvasContext->addConnectWireContext(newConnectorCtx);
            graphics->setSelected(true);
        }
    }

    for each (auto source in newSourcesMap) {
        if (source->prototypeName() == "Busbar" || source->prototypeName() == "DotBusbar") {
            auto trans = canvasScene->getTransformProxyGraphicsByID(source->uuid());
            if (trans) {
                for each (auto port in trans->getPortGraphicsObjectList()) {
                    if (port->getLinkedConnectorWireList().isEmpty()) {
                        source->delPortContext(port->getPortContext());
                        auto sourceGraphicsObject = trans->getSourceGraphicsObject();
                        if (sourceGraphicsObject) {
                            sourceGraphicsObject->deletePortGraphicsObject(port);
                        }
                    }
                }
            }
        }
    }

    if (canvasDefault) {
        // 把创建的新图元设置成图元组
        canvasDefault->makeSelectedItemAsGroup(canvasDefault->selectedItems());
    }
    startVerifyCanvas();

    //canvasScene->refreshInputAndOutPutIndex();
}

void PasteCommand::undo()
{
    if (!canvasScene) {
        return;
    }
    if (canvasScene->getTransformItemGroup()) {
        canvasScene->getTransformItemGroup()->clear();
    }
    canvasScene->clearSelection();
    // 先删除连接线
    QMapIterator<QString, PConnectorWireContext> iterConnector(newConnectorsMap);
    while (iterConnector.hasNext()) {
        iterConnector.next();
        QString uuid = iterConnector.key();
        ConnectorWireGraphicsObject *connectorGraphics = canvasScene->getConnectorWireGraphicsByID(uuid);
        if (connectorGraphics) {
            canvasScene->deleteConnectorWireGraphicsObject(connectorGraphics);
        }
    }

    QMapIterator<QString, PSourceProxy> iter(newSourcesMap);
    while (iter.hasNext()) {
        iter.next();
        QString uuid = iter.key();
        PSourceProxy source = iter.value();
        TransformProxyGraphicsObject *proxyGraphics = canvasScene->getTransformProxyGraphicsBySource(source);
        canvasScene->deleteTransformProxyGraphicsObject(proxyGraphics);
    }
    isUndoRun = true;
    startVerifyCanvas();
    //canvasScene->refreshInputAndOutPutIndex();
}

bool PasteCommand::sortInOutSource(const PSourceProxy source1, const PSourceProxy source2)
{
    if (!source1 || !source2) {
        return false;
    }
    SourceProperty &sourceProperty1 = source1->getSourceProperty();
    SourceProperty &sourceProperty2 = source2->getSourceProperty();
    qDebug() << sourceProperty1.toJson();
    int source1Index = sourceProperty1.getInputOutputIndex();
    int source2Index = sourceProperty2.getInputOutputIndex();

    return source1Index < source2Index;
}
