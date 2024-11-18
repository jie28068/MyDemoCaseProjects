#pragma once
#include "CanvasUndoCommand.h"
#include "ConnectorWireContext.h"
#include "PortContext.h"

class AddConnectorWireCommand : public CanvasUndoCommand
{
public:
    AddConnectorWireCommand(ICanvasScene *canvas, QUndoCommand *parent = 0);

    void addConnectorWireContext(PConnectorWireContext ctx);

protected:
    void undo();

    void redo();

    void createConnectorWires();

    void deleteConnectorWires();

    void addPortContext(PPortContext portCtx, QString sourceUUID);

private:
    QMap<QString, PConnectorWireContext> connectors;

    QMap<QString, PPortContext> portMap; // key为SourceID+PortID
};

/// @brief 连接线开关切换command
class ConnectorWireSwitchCommand : public CanvasUndoCommand
{
public:
    ConnectorWireSwitchCommand(ICanvasScene *canvas, PConnectorWireContext context, bool switchValue,
                               QUndoCommand *parent = 0);

protected:
    void redo();

    void undo();

private:
    bool swtichValue;
    PConnectorWireContext connectorContext;
};

class ConnectorWireAdjustCommand : public CanvasUndoCommand
{
public:
    ConnectorWireAdjustCommand(ICanvasScene *canvas, QString wireId, PConnectorWireContext oldContext,
                               PConnectorWireContext newContext, QUndoCommand *parent = 0);

protected:
    void redo();

    void undo();

private:
    PConnectorWireContext oldWireContext;
    PConnectorWireContext newWireContext;
    QString connectorWireId;
};
