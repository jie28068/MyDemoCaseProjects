#pragma once

#include "CanvasContext.h"
#include "Global.h"

#include <QMimeData>
#include <QSharedPointer>

class SourceProxy;
class ConnectorWireContext;
class GraphicsLayer;

class GRAPHICSMODELINGKERNEL_EXPORT ClipBoardMimeData : public QMimeData
{
public:
    ClipBoardMimeData(CanvasContext::Type type);

    void addGraphicsLayer(GraphicsLayer *layer);
    CanvasContext::Type getCanvasType();
    QMap<QString, QSharedPointer<SourceProxy>> getSourcesMap();

    QMap<QString, QSharedPointer<ConnectorWireContext>> getConnectorsMap();
    bool IsContainSource(QString sourceID); // 剪贴板sourcesMap中是否包含某Souerce

protected:
    void addSourceProxy(QSharedPointer<SourceProxy> source);

    void addConnectorWireContext(QSharedPointer<ConnectorWireContext> connectorCtx);

private:
    QMap<QString, QSharedPointer<SourceProxy>> sourcesMap;

    QMap<QString, QSharedPointer<ConnectorWireContext>> connectorsMap;

    CanvasContext::Type canvasType;
};
