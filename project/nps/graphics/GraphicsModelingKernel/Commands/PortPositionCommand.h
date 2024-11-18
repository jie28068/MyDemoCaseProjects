#pragma once

#include "CanvasUndoCommand.h"
#include "ConnectorWireContext.h"
#include "PortContext.h"
#include "PortGraphicsObject.h"

// 端口位置调节
class PortPositionCommand : public CanvasUndoCommand
{
public:
    PortPositionCommand(ICanvasScene *canvas, QString proxyGraphicsID, PPortContext oldPortContext,
                        PPortContext newPortContext, QList<PConnectorWireContext> oldConnectorWireContexts,
                        QList<PConnectorWireContext> newConnectorWireContexts, QUndoCommand *parent = 0);

protected:
    void redo();
    void undo();

private:
    QString proxyGraphicsID;
    PPortContext oldPortContext;
    QList<PConnectorWireContext> oldConnectorWireContexts;

    PPortContext newPortContext;
    QList<PConnectorWireContext> newConnectorWireContexts;
};
