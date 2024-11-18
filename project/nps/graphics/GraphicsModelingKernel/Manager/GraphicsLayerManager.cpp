#include "GraphicsLayerManager.h"
#include "GraphicsLayerManagerPrivate.h"
#include "ICanvasScene.h"
#include "Utility.h"

GraphicsLayerManager::GraphicsLayerManager(QSharedPointer<CanvasContext> canvasCtx)
{
    dataPtr.reset(new GraphicsLayerManagerPrivate);
    dataPtr->canvasCtx = canvasCtx;
}

GraphicsLayerManager::~GraphicsLayerManager() { }

QList<GraphicsLayer *> GraphicsLayerManager::getLayers(int flag)
{
    QList<GraphicsLayer *> results;
    QList<int> indexs = Utility::getBinaryOneFlag(flag);
    for each (int pos in indexs) {
        if (pos >= layerMaxIndex || pos < 0) {
            continue;
        }
        results.append(dataPtr->layersList[pos]);
    }
    return results;
}

QList<GraphicsLayer *> GraphicsLayerManager::getAllLayers()
{
    QList<GraphicsLayer *> results;
    for each (QList<GraphicsLayer *> layerList in dataPtr->layersList) {
        for (int index = 0; index < layerList.size(); index++) {
            results.append(dataPtr->layersList->at(index));
        }
    }
    return results;
}

void GraphicsLayerManager::addLayer(QList<int> indexs, GraphicsLayer *layer)
{
    if (nullptr == layer) {
        return;
    }
    for each (int pos in indexs) {
        if (pos >= layerMaxIndex || pos < 0) {
            continue;
        }
        dataPtr->layersList[pos].append(layer);
    }
}

void GraphicsLayerManager::removeLayer(QList<int> indexs, GraphicsLayer *layer)
{
    if (nullptr == layer) {
        return;
    }
    for each (int pos in indexs) {
        if (pos >= layerMaxIndex || pos < 0) {
            continue;
        }
        dataPtr->layersList[pos].removeAll(layer);
    }
}
