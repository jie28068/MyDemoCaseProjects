#pragma once

#include "CanvasUndoCommand.h"
#include "ConnectorWireContext.h"
#include "PortContext.h"
#include "SourceProxy.h"

class GraphicsLayer;

class DeleteCommand : public CanvasUndoCommand
{
public:
    DeleteCommand(ICanvasScene *scene, QUndoCommand *parent = 0);

    void addDeleteGraphics(GraphicsLayer *layer);

protected:
    void redo();

    void undo();

private:
    void addSourceProxy(PSourceProxy source);

    void addConnectorWireContext(PConnectorWireContext connectorCtx);

    void addPortContext(PPortContext portCtx, QString sourceUUID);

    void addConnectWirePort(PConnectorWireContext connectorCtx); // 连接线的端口

private:
    QMap<QString, PSourceProxy> sourcesMap;

    QMap<QString, PConnectorWireContext> connectorsMap;

    QMap<QString, PPortContext> portMap; // keyΪSourceID+PortID
};
