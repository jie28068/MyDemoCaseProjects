#pragma once

#include "CanvasUndoCommand.h"
#include "ConnectorWireContext.h"
#include "SourceProxy.h"

class SwitchBusbarTypeCommand : public CanvasUndoCommand
{
public:
    SwitchBusbarTypeCommand(ICanvasScene *scene, QUndoCommand *parent = 0);

    ~SwitchBusbarTypeCommand();

    void setSourcePorxy(PSourceProxy sourceProxy);

protected:
    void redo();

    void undo();

private:
    PSourceProxy originalSourceProxy;
    PSourceProxy newSourceProxy;
};
