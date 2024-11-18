#include "SourceProxyCommand.h"
#include "TransformItemGroup.h"

AddSourceProxyCommand::AddSourceProxyCommand(ICanvasScene *scene, QUndoCommand *parent /* = 0 */)
    : CanvasUndoCommand(scene, parent)
{
    ignoreRedo = true;
}

AddSourceProxyCommand::~AddSourceProxyCommand() { }

void AddSourceProxyCommand::addSourceProxy(PSourceProxy source)
{
    if (source.isNull()) {
        return;
    }
    QString uuid = source->uuid();
    if (sourcesMap.contains(uuid)) {
        return;
    }
    sourcesMap[uuid] = source;
}

void AddSourceProxyCommand::createSources()
{
    if (!canvasScene) {
        return;
    }
    QMapIterator<QString, PSourceProxy> iter(sourcesMap);
    while (iter.hasNext()) {
        iter.next();
        QString uuid = iter.key();
        PSourceProxy source = iter.value();
        source->setSourceCreateType(SourceProxy::kCreateRevert);
        canvasScene->loadFromSourceProxy(source);
    }
}

void AddSourceProxyCommand::deleteSources()
{
    if (!canvasScene) {
        return;
    }
    if (canvasScene->getTransformItemGroup()) {
        canvasScene->getTransformItemGroup()->clear();
    }
    QMapIterator<QString, PSourceProxy> iter(sourcesMap);
    while (iter.hasNext()) {
        iter.next();
        QString uuid = iter.key();
        PSourceProxy source = iter.value();
        TransformProxyGraphicsObject *proxyGraphics = canvasScene->getTransformProxyGraphicsBySource(source);
        if (proxyGraphics) {
            auto ports = proxyGraphics->getPortGraphicsObjectList();
            foreach (auto port, ports) {
                auto wires = port->getLinkedConnectorWireList();
                foreach (auto wire, wires) {
                    canvasScene->deleteConnectorWireGraphicsObject(wire);
                }
            }
            canvasScene->deleteTransformProxyGraphicsObject(proxyGraphics);
        }
    }
}

void AddSourceProxyCommand::undo()
{
    deleteSources();
    startVerifyCanvas();
    //canvasScene->refreshInputAndOutPutIndex();
}

void AddSourceProxyCommand::redo()
{
    if (ignoreRedo) {
        ignoreRedo = false;
        bool isNeedVerify = false;
        QMapIterator<QString, PSourceProxy> iter(sourcesMap);
        while (iter.hasNext()) {
            iter.next();
            PSourceProxy source = iter.value();
            if (source->moduleType() == GKD::SOURCE_MODULETYPE_ELECTRICAL) {
                isNeedVerify = true;
                break;
            }
        }
        if (isNeedVerify) {
            startVerifyCanvas();
        }
        //canvasScene->refreshInputAndOutPutIndex();
        return;
    }
    createSources();
    startVerifyCanvas();
    //canvasScene->refreshInputAndOutPutIndex();
}