#pragma once
#include "CanvasUndoCommand.h"
#include "SourceProxy.h"

class ControlCopyCommand : public CanvasUndoCommand
{
public:
    ControlCopyCommand(ICanvasScene *scene, QUndoCommand *parent = 0);
    ~ControlCopyCommand();

    void addSourceProxy(PSourceProxy source);

    void addConnectorWire(PConnectorWireContext wireContext);

protected:
    void undo();

    void redo();

private:
    void createSources();

    void deleteSources();

    void createConnectorWires();

    void deleteConnectorWires();

private:
    /// @brief 资源对象
    QMap<QString, PSourceProxy> sourcesMap;

    QMap<QString, PConnectorWireContext> connectorWiresMap;

    QMap<QString, QList<PPortContext>> portContextMap;
};
