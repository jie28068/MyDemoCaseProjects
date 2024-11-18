#pragma once
#include "GraphicsLayer.h"

#include <QSet>

const int layerMaxIndex = 32;
class GraphicsLayerManagerPrivate
{
public:
    QList<GraphicsLayer *> layersList[layerMaxIndex];
    QSharedPointer<CanvasContext> canvasCtx;
};