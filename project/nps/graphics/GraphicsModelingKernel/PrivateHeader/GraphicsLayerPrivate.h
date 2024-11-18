#pragma once
#include "GraphicsLayer.h"
#include "NPSPropertyManager.h"
#include "Utility.h"

class ICanvasScene;
class GraphicsLayerPrivate
{
public:
    GraphicsLayerPrivate();
    ~GraphicsLayerPrivate();

    ICanvasScene *canvasScene;
    GraphicsLayer::GraphicsLayerFlag layerFlag;
    GraphicsLayerProperty properties;
    QString uuid;
    bool isMovingCleanBranchPoints;
    QGraphicsItem *parentItem;
};

GraphicsLayerPrivate::GraphicsLayerPrivate()
{
    layerFlag = GraphicsLayer::kInvalidGraphicsLayer;
    uuid = Utility::createUUID();
    isMovingCleanBranchPoints = false;
    parentItem = nullptr;
}

GraphicsLayerPrivate::~GraphicsLayerPrivate() { }